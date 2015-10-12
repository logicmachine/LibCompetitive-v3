import sys, os, re, time, subprocess, argparse, configparser, logging
import requests, requests.utils, pickle

#----------------------------------------------------------------------------
# Argument parser
#----------------------------------------------------------------------------
parser = argparse.ArgumentParser()
parser.add_argument('-v', '--verbose', action='store_true', help='Turn on verbose output.')
sub_parsers = parser.add_subparsers(dest='subcommand')

init_parser = sub_parsers.add_parser('init', help='Authorize and fetch materials of the contest.')
init_parser.add_argument('URL', help='An URL of the contest page.')

test_parser = sub_parsers.add_parser('test', help='Test a solution by example cases.')
test_parser.add_argument('problem', help='An identifier of the problem.')
test_parser.add_argument('command', help='A command to run a solution.')

submit_parser = sub_parsers.add_parser('submit', help='Submit a solution.')
submit_parser.add_argument('problem', help='An identifier of the problem.')
submit_parser.add_argument('source', help='A source file that will be submitted.')

#----------------------------------------------------------------------------
# HTTP client
#----------------------------------------------------------------------------
class HttpClient:
	def __init__(self):
		self.session_file = os.path.join(os.environ['HOME'], '.cli-contest-session')
		self.session = requests.session()
		if os.path.exists(self.session_file):
			with open(self.session_file, 'rb') as f:
				cookies = requests.utils.cookiejar_from_dict(pickle.load(f))
				self.session.cookies = cookies

	def query_get(self, url, data=None):
		r = self.session.get(url, params=data)
		return (r.status_code, r.url, r.headers, r.text)

	def query_post(self, url, data=None, files=None):
		r = self.session.post(url, params=data, files=files)
		return (r.status_code, r.url, r.headers, r.text)

	def store_session(self):
		cookiejar = requests.utils.dict_from_cookiejar(self.session.cookies)
		with open(self.session_file, 'wb') as f:
			pickle.dump(cookiejar, f)

#----------------------------------------------------------------------------
# Utility functions
#----------------------------------------------------------------------------
def parse_url(url):
	match_atcoder = re.match(r'http[s]?://([^\.]+)\.contest\.atcoder\.jp', url)
	match_codeforces = re.match(r'http[s]?://codeforces.com/contest/(\d+)', url)
	match_hackerrank = re.match(r'http[s]?://www.hackerrank.com/contests/([^/]+)', url)
	if match_atcoder is not None:
		return ('atcoder', match_atcoder.group(1))
	elif match_codeforces is not None:
		return ('codeforces', match_codeforces.group(1))
	elif match_hackerrank is not None:
		return ('hackerrank', match_hackerrank.group(1))
	else:
		logging.error('Failed to parse the contest URL')
		exit(-1)

def load_module(site, contest, http_client, global_config, local_config):
	module = __import__(site)
	klass = getattr(module, 'Contest')
	return klass(contest, http_client, global_config, local_config)

#----------------------------------------------------------------------------
# Test runner
#----------------------------------------------------------------------------
def compare_outputs(actual, expected):
	return (actual == expected, [])

def run_tests(module, problem, command):
	passed_str = '\033[32m[ PASSED ]\033[m'
	failed_str = '\033[31m[ FAILED ]\033[m'
	examples = module.get_examples(problem)
	if examples is None:
		logging.error('Problem {} is not exists on this contest.'.format((problem)))
		return
	num_passed = 0
	num_failed = 0
	for name, input_data, output_data in examples:
		begin_time = time.time()
		proc = subprocess.Popen(
			command, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		result = proc.communicate(input_data.encode())[0].decode()
		end_time = time.time()
		elapsed_time = end_time - begin_time
		compare_result = compare_outputs(result, output_data)
		is_passed = compare_result[0]
		messages = []
		if elapsed_time > 0.1:
			messages.append('{0:.0f} ms'.format(elapsed_time * 1000))
		if proc.returncode != 0:
			is_passed = False
			messages.append('return code is not 0')
		messages.extend(compare_result[1])
		formatted_line = '{0} test case {1}'.format(
			passed_str if is_passed else failed_str, name)
		if len(messages) > 0:
			formatted_line += ' (' + ', '.join(messages) + ')'
		print(formatted_line)
		if not is_passed:
			num_failed += 1
			print('Input:')
			print(input_data)
			print('Expected output:')
			print(output_data)
			print('Actual output:')
			print(result)
		else:
			num_passed += 1
	num_cases = num_passed + num_failed
	if num_failed == 0:
		print('{0} {1} tests'.format(passed_str, num_passed))
	else:
		print('{0} {1} of {2} tests'.format(failed_str, num_failed, num_cases))

#----------------------------------------------------------------------------
# Code preprocessor
#----------------------------------------------------------------------------
def preprocess(filename, global_config):
	if 'Common' not in global_config:
		global_config['Common'] = {}
	ext = os.path.splitext(filename)[1][1:]
	preprocess_lang_key = 'preprocess_{0}'.format(ext)
	command = None
	if preprocess_lang_key in global_config['Common']:
		command = global_config['Common'][preprocess_lang_key]
	if command is None or command == 'default':
		# read the source file directly
		with open(filename) as f:
			return f.read()
	elif command == 'minifier':
		# call minifier
		proc = subprocess.Popen(['minifier', filename], stdout=subprocess.PIPE)
		output = proc.communicate()[0]
		if proc.returncode != 0:
			return None
		if len(output) == 0:
			return None
		return output.decode('utf-8')
	else:
		raise ValueError('unknown preprocessor')

#----------------------------------------------------------------------------
# main
#----------------------------------------------------------------------------
if __name__ == '__main__':
	args = parser.parse_args()
	logging.basicConfig(
		format='%(levelname)s: %(message)s',
		level=(logging.DEBUG if args.verbose else logging.WARNING))
	# load global configurations
	global_config_file = os.path.join(os.environ['HOME'], '.cli-contest.ini')
	global_config = configparser.ConfigParser()
	if os.path.exists(global_config_file):
		with open(global_config_file) as f:
			global_config.read_file(f)
	# initialize common components
	local_config = configparser.ConfigParser()
	http_client = HttpClient()
	# subcommand
	if args.subcommand == 'init':
		# initialize directory
		url_data = parse_url(args.URL)
		local_config['Common'] = {
			'site':    url_data[0],
			'contest': url_data[1],
			'url':     args.URL
		}
		module = load_module(
			url_data[0], url_data[1], http_client, global_config, local_config)
		module.initialize()
	else:
		# load local configurations
		try:
			with open('.cli-contest.ini') as f:
				local_config.read_file(f)
		except:
			logging.error('This directory is not initialized.')
			sys.exit(1)
		module = load_module(
			local_config['Common']['site'], local_config['Common']['contest'],
			http_client, global_config, local_config)
		if args.subcommand == 'test':      # test a compiled solution
			if module.has_problem(args.problem):
				run_tests(module, args.problem, args.command)
			else:
				logging.error('Problem {0} does not exist in this contest.'.format(args.problem))
				sys.exit(2)
		elif args.subcommand == 'submit':  # submit a solution
			if module.has_problem(args.problem):
				ext = os.path.splitext(args.source)[1][1:]
				code = preprocess(args.source, global_config)
				module.submit(args.problem, code, ext)
			else:
				logging.error('Problem {0} does not exist in this contest.'.format(args.problem))
				sys.exit(2)
	# save modified configurations
	http_client.store_session()
	with open('.cli-contest.ini', 'w') as f:
		local_config.write(f)
	with open(global_config_file, 'w') as f:
		global_config.write(f)

