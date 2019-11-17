#!/usr/bin/env python
import glob, re, pprint
import os, sys
import datetime
import argparse
import subprocess

''' merge_all_dot_files assumes all .dot files to be merged are in the current directory'''
def merge_all_dot_files(outFileName):
    graphNodes = {}
    graphEdges = {}
    filesCount = 1;

    for file in os.listdir("./"):
        if file.endswith(".dot"):
            oldNumToNewNum = {}
            f = open(file, mode='r')
            lines = f.readlines()
            f.close()
            for line in lines:
                oldNumMatch = re.search('^ [0-9]*', line)
                if oldNumMatch != None:
                    oldNum = int(oldNumMatch.group()[1:])
                    labelMatch = re.search('\[label=\".*?\"', line)
                    if labelMatch != None:
                        '''line defining a node'''
                        labelStr = labelMatch.group()
                        key = labelStr[len('[label="'):-1]
                        if not key in graphNodes:
                            newNumStr = ' {} '.format(filesCount)
                            newLine = re.sub("^ [0-9]* ", newNumStr, line)
                            oldNumToNewNum[oldNum] = filesCount
                            graphNodes[key] = newLine
                            filesCount = filesCount + 1
                        else:
                            ''''node was added by a previously scanned file'''
                            newNumMatch = re.search("^ [0-9]* ", graphNodes[key])
                            if newNumMatch != None:
                                oldNumToNewNum[oldNum] = int(newNumMatch.group()[1:])
                    else:
                        '''line defining edges from a node'''
                        edgesListMatch = re.search('\{ .* \}', line)
                        if edgesListMatch != None:
                            newNum = oldNumToNewNum[oldNum]
                            if not newNum in graphEdges:
                                edgesListStr = edgesListMatch.group()[2:-2]
                                edgesList = edgesListStr.split()
                                graphEdges[newNum] = set()
                                for edge in edgesList:
                                    graphEdges[newNum].add(oldNumToNewNum[int(edge)])
        dot_file = outFileName + '.dot'
        outFile = open(dot_file, mode='w')
        outFile.write("digraph d {\n")

        for node in graphNodes.values():
             outFile.write(node)

        for node, neighbors in graphEdges.items():
            nodeStr = " {0} -> {{ ".format(node)
            outFile.write(nodeStr)
            for edge in neighbors:
                outFile.write('{} '.format(edge))
            outFile.write('}\n')
        outFile.write('}')
        outFile.close()

        createDependenciesGraph_png(dot_file, outFileName)

def createDependenciesGraph_png(dot_file, png_file_name):
    try:
        subprocess.call('dot -T png -o' + png_file_name + '.png ' + dot_file, shell=True)
    except OSError as err:
        print("Unable to run dependencies mapping plugin")
        print(err)
        sys.exit(1)

def removeFiles(files_lst):
    for file in files_lst:
      try:
          os.remove(file)
      except OSError as err:
          print("Unable to remove auxiliary file: ", file)
          print(err)
          sys.exit(1)

''' create a .dot file for each file in the list of input files and a .png graph, if defined in args '''
def gen_dot_files_and_dependencies_graph(args, dot_dir_path):
    dot_files = []
    for file in open(args.file_with_inputs[0]):
        file = file.strip()
        file_name_no_ext = os.path.splitext(os.path.basename(file))[0]
        dot_file = os.path.join(dot_dir_path, '.' + file_name_no_ext + '.dot')

        command = '-cc1 -load ' + os.path.join(args.plugin[0], 'DependenciesMappingPlugin.dylib') + ' -plugin DependenciesMappingPlugin ' + '-plugin-arg-DependenciesMappingPlugin -out-file ' + '-plugin-arg-DependenciesMappingPlugin ' + dot_file + ' '
        if file.endswith('cpp'):
            command = os.path.join(args.clang[0], 'clang++') + ' ' + command + file + ' -std=c++11'
        else:
            command = os.path.join(args.clang[0], 'clang') + ' ' + command + file

        ''' add to dot file for removal '''
        dot_files.append(dot_file)

        try:
            print("=== Creating dependencies map for: " + os.path.basename(file) + " ===")
            subprocess.call(command, shell=True)
            if args.subgraphs == True:
                print("=== Generating dependencies grpah (.png) for: " + os.path.basename(file) + " ===")
                createDependenciesGraph_png(dot_file, file_name_no_ext + '_DependenciesGraph')
        except OSError as err:
            print(err)
            print("Unable to run dependencies mapping plugin")
            sys.exit(1)
    return dot_files

def main():
  parser = argparse.ArgumentParser(description='Generates file dependencies graphs')
  parser.add_argument('file_with_inputs', nargs='+', help='file containing a list with all the files in project (full path)')
  parser.add_argument('-gen-subgraphs', nargs='?', dest='subgraphs', default=False, const=True, help='wether to generate dependencies graphs for single files in the project')
  parser.add_argument('-clang-dir', dest='clang', nargs='+', default='.', help='path to clang/clang++')
  parser.add_argument('-plugin-dir', dest='plugin', nargs='+', default='.', help='path to plugin dynamic library')
  args = parser.parse_args()

  python_dir = os.path.dirname(os.path.realpath(__file__))
  ''' get timestamp to create a directory with a unique name '''
  now = datetime.datetime.now()
  dot_dir = 'dependenciesGraph_' + str(now.day) + str(now.month) + str(now.year) + '_' + str(now.hour) + str(now.minute)
  dot_dir_full_path = os.path.join(python_dir, dot_dir)

  try:
      if not os.path.isdir(dot_dir_full_path):
          os.makedirs(dot_dir_full_path)
      os.chdir(dot_dir_full_path)
  except OSError as err:
      print(err)
      sys.exit(1)

  dot_dir_full_path = os.path.join(python_dir, dot_dir)
  dot_files = gen_dot_files_and_dependencies_graph(args, dot_dir_full_path)
  outFileName = 'DependenciesGraph_AllFiles'
  dot_file = os.path.join(dot_dir_full_path, outFileName)
  ''' add to dot file for removal '''
  dot_files.append(dot_file + '.dot')
  merge_all_dot_files(dot_file)
  removeFiles(dot_files)

if __name__ == '__main__':
  main()
