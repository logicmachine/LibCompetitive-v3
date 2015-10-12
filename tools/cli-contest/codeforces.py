import os, re, configparser, logging, getpass, lxml.html, lxml.etree, urllib.parse

DEFAULT_LANGUAGES = {
	'c':     'GNU GCC C11 4.9.2',
	'cpp':   'GNU G++11 4.9.2',
	'cxx':   'GNU G++11 4.9.2',
	'cs':    'MS C# .NET 4',
	'd':     'D DMD32 Compiler v2',
	'go':    'Go 1.2',
	'hs':    'Haskell GHC 7.6',
	'java':  'Java 8',
	'ml':    'OCaml 4',
	'pas':   'Free Pascal 2',
	'pl':    'Perl 5.12',
	'php':   'PHP 5.3',
	'py':    'Python 3.4',
	'rb':    'Ruby 2',
	'scala': 'Scala 2.11',
	'js':    'JavaScript V8 3'
}

class NotAuthorizedError(Exception):
	def __init__(self):
		Exception.__init__(self)

class ProblemNotFoundError(Exception):
	def __init__(self):
		Exception.__init__(self)

class LanguageNotFoundError(Exception):
	def __init__(self):
		Exception.__init__(self)

def input_user_info():
	username = input('Username: ')
	password = getpass.getpass('Password: ')
	return (username, password)

def stringify_node(node):
	return node.text + ''.join(lxml.etree.tostring(e).decode('utf-8') for e in node)

class Contest:
	def __initialize_global_config(self):
		section = 'Codeforces'
		if section not in self.global_config:
			self.global_config[section] = {}
		for ext, name in DEFAULT_LANGUAGES.items():
			entry_name = 'language_{0}'.format(ext)
			if entry_name not in self.global_config[section]:
				self.global_config[section][entry_name] = name

	def __is_authorized(self, dom_root):
		header_texts = dom_root.xpath(
			'//*[@id="header"]/div[@class="lang-chooser"]//text()')
		has_enter = False
		has_register = False
		for text in header_texts:
			if text.strip() == 'Enter':
				has_enter = True
			elif text.strip() == 'Register':
				has_register = True
		return not (has_enter and has_register)

	def __fill_form_default(self, form_root):
		form_data = {}
		form_files = {}
		for elem in form_root.xpath('//input'):
			if 'name' not in elem.attrib:
				continue
			if 'type' in elem.attrib and elem.attrib['type'] == 'file':
				form_files[elem.attrib['name']] = ('', '')
			elif 'value' in elem.attrib:
				form_files[elem.attrib['name']] = elem.attrib['value']
			else:
				form_files[elem.attrib['name']] = ''
		for elem in form_root.xpath('//select'):
			if 'name' not in elem.attrib:
				continue
			option = elem.xpath('option[@selected]')
			if len(option) > 0:
				form_data[elem.attrib['name']] = option[0].attrib['value']
			else:
				form_data[elem.attrib['name']] = ''
		for elem in form_root.xpath('//textarea'):
			if 'name' not in elem.attrib:
				continue
			form_data[elem.attrib['name']] = ''
			text_elem = elem.xpath('text()')
			if len(text_elem) > 0:
				form_data[elem.attrib['name']] = text_elem[0]
		if len(form_files) == 0:
			form_files = None
		return (form_data, form_files)

	def __authenticate(self):
		enter_url = 'http://codeforces.com/enter'
		enter_response = self.http_client.query_get(enter_url)
		dom_root = lxml.html.fromstring(enter_response[3])
		while not self.__is_authorized(dom_root):
			dom_form = dom_root.xpath('//form[@id="enterForm"]')
			(form_data, form_files) = self.__fill_form_default(dom_form)
			user_info = input_user_info()
			form_data['handle'] = user_info[0]
			form_data['password'] = user_info[1]
			form_data['remember'] = '1'
			enter_response = self.http_client.query_post(enter_url, form_data)
			dom_root = lxml.html.fromstring(enter_response[3])

	def __http_get(self, url, data=None):
		logging.debug('GET {0}'.format(url))
		response = self.http_client.query_get(url, data)
		dom_root = lxml.html.fromstring(response[3])
		if not self.__is_authorized(dom_root):
			raise NotAuthorizedError()
		return response

	def __http_post(self, url, data=None, files=None):
		logging.debug('POST {0}'.format(url))
		response = self.http_client.query_post(url, data, files)
		dom_root = lxml.html.fromstring(response[3])
		if not self.__is_authorized(dom_root):
			raise NotAuthorizedError()
		return response

	def __load_problem_list(self):
		contest_url = 'http://codeforces.com/contest/{}'.format(self.contest_id)
		problems = dict()
		while True:
			contest_response = None
			try:
				contest_response = self.__http_get(contest_url)
			except NotAuthorizedError:
				self.__authenticate()
				continue
			dom_root = lxml.html.fromstring(contest_response[3])
			dom_rows = dom_root.xpath('//*[@id="pageContent"]//table[@class="problems"]//tr')
			for dom_row in dom_rows:
				dom_cols = dom_row.xpath('td')
				if len(dom_cols) == 0:
					continue
				problem_id = dom_cols[0].xpath('a/text()')[0].strip()
				description_url = urllib.parse.urljoin(
					contest_response[1], dom_cols[0].xpath('a/@href')[0])
				submission_url = urllib.parse.urljoin(
					contest_response[1], dom_cols[2].xpath('span/a/@href')[0])
				problems[problem_id] = (description_url, submission_url)
			break
		return problems

	def __read_examples(self, description_url):
		examples = []
		while True:
			description_response = None
			try:
				description_response = self.__http_get(description_url)
			except NotAuthorizedError:
				self.__authenticate()
				continue
			dom_root = lxml.html.fromstring(description_response[3])
			dom_inputs = dom_root.xpath('//div[@class="sample-test"]/div[@class="input"]/pre')
			dom_outputs = dom_root.xpath('//div[@class="sample-test"]/div[@class="output"]/pre')
			for in_node, out_node in zip(dom_inputs, dom_outputs):
				examples.append((
					stringify_node(in_node).replace('<br/>', '\n').strip() + '\n',
					stringify_node(out_node).replace('<br/>', '\n').strip() + '\n'))
			break
		return examples

	def __parse_language_list(self, dom_select):
		languages = {}
		for elem in dom_select.xpath('option'):
			name = stringify_node(elem).strip()
			value = elem.attrib['value']
			languages[name] = value
		return languages


	def __init__(self, contest_id, http_client, global_config, local_config):
		self.contest_id = contest_id
		self.http_client = http_client
		self.global_config = global_config
		self.local_config = local_config
		self.__initialize_global_config()


	def initialize(self):
		problems = self.__load_problem_list()
		for key, value in problems.items():
			config_section = 'Problem-' + key
			self.local_config[config_section] = { 'submit_url': value[1] }
			examples = self.__read_examples(value[0])
			if not os.path.exists(key):
				os.mkdir(key, 0o755)
			index = 0
			for example in examples:
				in_filename = os.path.join(key, '{0:02d}.in'.format(index))
				out_filename = os.path.join(key, '{0:02d}.out'.format(index))
				with open(in_filename, 'w') as f:
					f.write(example[0])
				with open(out_filename, 'w') as f:
					f.write(example[1])
				index += 1


	def has_problem(self, problem_id):
		config_section = 'Problem-{0}'.format(problem_id)
		return config_section in self.local_config


	def get_examples(self, problem):
		config_section = 'Problem-' + problem
		if config_section not in self.local_config:
			raise ProblemNotFoundError()
		input_re = re.compile(r'(.*)\.in')
		examples = []
		for filename in os.listdir(problem):
			match = input_re.match(filename)
			if match is None:
				continue
			input_data = None
			output_data = None
			with open(os.path.join(problem, filename)) as f:
				input_data = f.read()
			output_filename = os.path.join(problem, match.group(1) + '.out')
			if os.path.exists(output_filename):
				with open(output_filename) as f:
					output_data = f.read()
			examples.append((match.group(1), input_data, output_data))
		return examples


	def submit(self, problem, solution, extension):
		config_section = 'Problem-' + problem
		if config_section not in self.local_config:
			raise ProblemNotFoundError()
		if 'submit_url' not in self.local_config[config_section]:
			raise ProblemNotFoundError()
		config_lang_key = 'language_{0}'.format(extension)
		if config_lang_key not in self.global_config['Codeforces']:
			raise LanguageNotFoundError()
		lang_name = self.global_config['Codeforces'][config_lang_key]

		submit_url = self.local_config[config_section]['submit_url']
		submit_response = self.__http_get(submit_url)
		dom_root = lxml.html.fromstring(submit_response[3])
		dom_form = dom_root.xpath('//*[@class="submit-form"]')[0]
		dom_select = dom_form.xpath('//select[@name="programTypeId"]')[0]
		langs = self.__parse_language_list(dom_select)
		if lang_name not in langs:
			raise LanguageNotFoundError()

		(form_data, form_files) = self.__fill_form_default(dom_form)
		form_data['programTypeId'] = langs[lang_name]
		form_data['submittedProblemIndex'] = problem
		form_data['source'] = solution

		form_action = dom_form.attrib['action']
		post_url = urllib.parse.urljoin(submit_response[1], form_action)
		submit_response = self.__http_post(post_url, form_data, form_files)
		print(submit_response)

