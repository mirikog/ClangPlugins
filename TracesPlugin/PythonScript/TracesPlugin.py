#!/usr/bin/env python
import argparse
import os
import subprocess
import sys

def group_input_files_according_to_traces_requirement(file_with_input_files):
    files_add_traces = []
    files_no_traces = []

    for line in open(file_with_input_files):
        line = line.strip()
        split_line = line.split()
        if len(split_line) > 1 and split_line[1] == 'T':
            files_add_traces.append(split_line[0])
        else:
            files_no_traces.append(split_line[0])
    return files_no_traces, files_add_traces

def generate_files_with_trace_prints(args, files_add_traces, isCPP):
    files_with_Trace = []
    for file in files_add_traces:
        root, ext = os.path.splitext(file)
        fileWithTraces = str(root) + '_traces' + str(ext)
        files_with_Trace.append(fileWithTraces)
        rewritting = '-cc1 -load ' + os.path.join(args.plugin[0], 'TracesPlugin.dylib') + ' -plugin TracesPlugin ' + '-plugin-arg-TracesPlugin -rewrittern-out ' + '-plugin-arg-TracesPlugin ' + fileWithTraces + ' ' + file
        if (isCPP):
            rewritting = os.path.join(args.clang[0], 'clang++') + ' ' + rewritting + ' -std=c++11'
        else:
            rewritting = os.path.join(args.clang[0], 'clang') + ' ' + rewritting
        try:
            print("== Inserting trace prints to " + file + " ==")
            subprocess.call(rewritting, shell=True)
        except:
            print("Unable to run traces plugin")
            sys.exit(1)
    return files_with_Trace

def compile_files(args, compilation_args, files, isCPP):
      compiling = ''
      for file in files:
          compiling = compiling + file + ' '
      #
      for compiler_arg in compilation_args:
          compiling = compiling + compiler_arg + ' '

      if (isCPP):
          compiling = os.path.join(args.clang[0], 'clang++') + ' ' + compiling + ' -std=c++11' + ' -stdlib=libstdc++'
      else:
          compiling = os.path.join(args.clang[0], 'clang') + ' ' + compiling

      try:
        print("=== Compiling files to an executable ===")
        subprocess.call(compiling, shell=True)
        print("=== Done ===")
      except:
        print("Unable to run traces plugin")
        sys.exit(1)

def main():
  parser = argparse.ArgumentParser(description='Generates an executable that includes trace prints')
  parser.add_argument('file_with_inputs', nargs='+', help='file containing a list with the files that are to be compiled, marking the files that should include trace prints')
  parser.add_argument('-keep-rewritten', nargs='?', dest='keep', default=False, const=True, help='wether to keep the generated source that includes trace prints')
  parser.add_argument('-clang-dir', dest='clang', nargs='+', default='.', help='path to clang/clang++')
  parser.add_argument('-plugin-dir', dest='plugin', nargs='+', default='.', help='path to plugin dynamic library')
  known_args, unknown_args = parser.parse_known_args()

  files_no_traces, files_add_traces = group_input_files_according_to_traces_requirement(known_args.file_with_inputs[0])

  isCPP = False
  if (files_no_traces):
      isCPP = files_no_traces[0].endswith('cpp')
  else:
      isCPP = files_add_traces[0].endswith('cpp')

  files_with_Trace = generate_files_with_trace_prints(known_args, files_add_traces, isCPP)
  compile_files(known_args, unknown_args, files_no_traces + files_with_Trace, isCPP)

  ''' remove rewritten fiels, if defined in args '''
  if known_args.keep == False:
      for file in files_with_Trace:
          try:
              os.remove(file)
          except:
              print("Unable to remove auxiliary file: ", file)
              sys.exit(1)

if __name__ == '__main__':
  main()
