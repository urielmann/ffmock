'''
    Extract exported symbols names
'''

# Copyright (C) 2019 Uriel Mann (abba.mann@gmail.com)

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import re
import subprocess
import argparse

EXPORTED_SYMBOLS_RE = re.compile(r'\s(\?(RealAPI|MockAPI)[^\s]+)\s')

def main():
    parser = argparse.ArgumentParser(allow_abbrev=False)

    # General options
    parser.add_argument('-i', '--input', help='Input file')
    parser.add_argument('-o', '--output', help='Output file')

    args = parser.parse_args()

    # get all mocks symbols
    results = subprocess.run(['dumpbin.exe', '/symbols', args.input], capture_output=True, text=True)

    exports = []
    # find symbols for MockAPI and RealAPI
    for line in results.stdout.split('\n'):
        match = re.search(EXPORTED_SYMBOLS_RE, line)
        if match:
            export = match.groups(1)[0]
            print(export)
            exports.append(export)
    print(exports)

if __name__ == '__main__':
    main()
