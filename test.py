#!/usr/bin/python

import os, sys
import fnmatch
import subprocess
import filecmp
import string

passCount = 0
failCount = 0
totalCount = 0

def testCode( retcode, msg ):
  if retcode > 0:
    print msg
    sys.exit( 1 )

testDir = os.path.join( os.getcwd(), 'cases')
if not os.path.isdir( testDir ):
  print testDir, "isn't a directory"
  sys.exit( 1 )

executable = os.path.join(os.getcwd(), "run")
if not os.path.isfile( executable ):
  retcode = subprocess.call("make",shell=True)
  testCode( retcode, "\tFAILED to make the scanner" )

files = os.listdir( testDir )
for x in files:
  if fnmatch.fnmatch(x, "*.py"):
    testcase = os.path.join(testDir, x)
    retcode = subprocess.call("python < "+testcase +"> /tmp/out2", shell=True)
    if retcode != 0:
      testCode( retcode, "\tFailed to run on Python. Syntactically incorrect test case or an error thrown by Python"+x)
    retcode = subprocess.call("./run < "+testcase+"> /tmp/out",shell=True)
    if retcode != 0:
      testCode( retcode, "\tFAILED to run test case "+x)
    else:
      if not os.path.isfile( "/tmp/out2" ):
        print "test case", x[:-3]+'.out', "doesn't exist"
        sys.exit( 1 )
      if not filecmp.cmp("/tmp/out", "/tmp/out2"): 
        print "\tTEST CASE FAILED", x
        failCount+=1
      else :
        print "\ntestcase:", x, "passed"
        os.system("cat "+testcase)
        passCount+=1

totalCount = passCount + failCount
result = '\n\tNumber of Cases: %d\n'%totalCount + \
 '\tCases Passed: %d\n'%passCount + \
  '\tCases Failed: %d\n'%failCount
print result

