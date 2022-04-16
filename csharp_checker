import re
import os
import sys
def SQLi(file_path,line,line_number):
    find_sql_command = re.findall(
        "(SELECT|INSERT INTO|UPDATE|DROP)"
        ,line,flags=re.IGNORECASE)
    if(find_sql_command): # Check if line contains sql statement
        if("@" in line and not "." in line): # check prepare and not email address
            return
        if(len(re.findall("\"",line)) > 2 and "+" in line):             
            print(f"{file_path} Found Sql on line {line_number}")
            return
        if(len(re.findall("\'",line)) > 2 and "+" in line): 
            print(f"{file_path} Found Sql on line {line_number}")
            return
        if("+" in line):
            print(f"{file_path} Found Sql on line {line_number}")
            return
        if("{" in line and "}" in line):
            print(f"{file_path} Found Sql on line {line_number}")
            return

def readfile(file):
   
    file_path = file
    file = open(file)
    line_number = 1   
    for line in file:
        SQLi(file_path,line,line_number)
        line_number += 1   

def main():
    path = "."
    try:
        path=sys.argv[1]
    except IndexError:
        pass
    list = []
    
    for (root, dirs, file) in os.walk(path):
        for f in file:
            ext = os.path.splitext(root+"\\"+f)
            if(ext[1] ==".cs" or ext[1]==".chtml"):
                # print(1)
                readfile(root+"\\"+f)
main()
