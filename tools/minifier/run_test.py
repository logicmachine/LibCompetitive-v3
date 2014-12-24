import os, sys, re, subprocess;

def print_success(name):
	print '\033[32m[ PASSED ]\033[m %s' % name

def print_failed(name):
	print '\033[31m[ FAILED ]\033[m %s' % name

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print 'Usage: python %s minifier_path [test_directory]' % sys.argv[0]
		quit()
	minifier_path = sys.argv[1]
	test_directory = os.path.dirname(os.path.abspath(__file__)) + '/test'
	if len(sys.argv) >= 3:
		test_directory = sys.argv[2]
	test_directory = os.path.abspath(test_directory)
	if test_directory[-1] != '/':
		test_directory += '/'
	matcher = re.compile(r'^(.*)\.in\.cpp$');
	test_inputs = []
	for directory in os.walk(test_directory):
		dir_path = directory[0]
		for filename in directory[2]:
			filepath = os.path.abspath(dir_path + '/' + filename)
			match = matcher.match(filepath)
			if match is not None:
				test_inputs.append(match.group(1))
	passed_tests = []
	failed_tests = []
	for filepath in test_inputs:
		test_name = filepath[len(test_directory):]
		input_path = filepath + '.in.cpp'
		expect_path = filepath + '.out.cpp'
		proc = subprocess.Popen([minifier_path, input_path], stdout=subprocess.PIPE)
		expect = ''.join(open(expect_path, 'rb').readlines())
		actual = proc.communicate()[0]
		if expect == actual:
			print_success(test_name)
			passed_tests.append(test_name)
		else:
			print_failed(test_name)
			failed_tests.append(test_name)
	if len(failed_tests) == 0:
		print_success('%d tests.' % len(passed_tests))
	else:
		print_failed('Passed %d tests and failed %d tests.' % (len(passed_tests), len(failed_tests)))

