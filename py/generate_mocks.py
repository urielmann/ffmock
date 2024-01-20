'''
    Extract Win32 API from header files and generate mock definition and implementation
'''

# Copyright (C) 2023-2024 Uriel Mann (abba.mann@gmail.com)

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

def main():
    parser = argparse.ArgumentParser(allow_abbrev=False,
                                     epilog='''
Generate mock(s) implementation for header file.
    If the script will search the input header (-i/--input) file for and API (-a/--api).
    the pattern is found, the definition and declaration of mock of this API will be
    placed in the mock files declaration (-c/--declaration) and definitions file
    (-d/--definition) files.
                                     ''')

    # General options
    parser.add_argument('-i', '--input', help='Input header file', required=True)
    parser.add_argument('-a', '--api', help='Input API name', required=True)
    parser.add_argument('-c', '--declaration', help='Output declaration file')
    parser.add_argument('-f', '--definition', help='Output definition file')

    args = parser.parse_args()

    APIS_RE = re.compile(rf'\b({args.api})\b')

    with open(args.input) as header:
        lines = header.readlines()

    apis = {}
    # find symbols for MockAPI and RealAPI
    for index, line in enumerate(lines):
        match = re.search(APIS_RE, line)
        if match:
            api = match.groups(1)[0]
            start = index - 1
            while lines[start].strip() != '':
                start = start - 1
            start = start + 1
            end = index + 1
            while lines[++end].strip() != '':
                end = end + 1
            apis.update({api: lines[start:end]})
    print(apis)

if __name__ == '__main__':
    main()
