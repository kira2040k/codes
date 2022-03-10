import socket
from re import findall,search
import requests
import paramiko
import ftplib
import matplotlib.pyplot as plt
from bs4 import BeautifulSoup 


    
class web():
    class webscraper:
        def __init__(self):
            self.url = ""
            self.text = ""
            self.soup = ""
            self.user_agent = {"User-Agent":"kira tools"}
        def set_user_agent(self,user_agent:str):
            self.user_agent = {"User-Agent":user_agent}
        def send_request(self,url:str):
            self.text = requests.get(url).text
            self.soup = BeautifulSoup(self.text,'lxml')
        def getall_response(self):
            return self.text    
        def get_urls(self):
            urls = findall("http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\(\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+",self.text)        
            return urls
        def check_value(self,value:str):
            if(value in self.text):
                return True
            else:
                return False 
        def value_by_class(self,tag:str,c:str):
            divs = self.soup.find_all(tag,{'class':c})
            list = []
            for div in divs:
                list.append(div.text)
            return list
        def value_by_id(self,tag:str,c:str):
            divs = self.soup.find_all(tag,{'id':c})
            list = []
            for div in divs:
                list.append(div.text)
            return list
        def value_by_tag(self,tag):
            divs = self.soup.find_all(tag)
            list = []
            for div in divs:
                list.append(div.text)
            return list
    class hack: 
        def fuzz(file:str,domain:str,ex:list=[""],black_list:list=[404]):
            FileName = file
            req_list = []
            statuscode_list = []
            len_list = []
            try:
                file = open(file,'r')
                for line in file:
                    line = line.strip('\n')
                    for e in ex:
                        line2 = domain.replace('FUZZ',line+"."+e)
                        r = requests.get(line2)
                        try:
                        
                            if(r.status_code in black_list):
                                pass
                            else:
                                req_list.append(line2)
                                statuscode_list.append(r.status_code)
                                len_list.append(int(len(r.content)))
                        except Exception as e:
                            print(e)        
                return req_list,statuscode_list,len_list            
            except FileNotFoundError:
                print("can't find "+FileName)
            except Exception as e:
                print(e)
        def wordpress_check(url:str):
            list = ['/wp-admin','/wp-includes','/license.txt','/readme.html'
            ,'/wp-activate.php','/wp-blog-header.php','/wp-comments-post.php',
            '/wp-config-sample.php','/wp-cron.php','/xmlrpc.php','/wp-login.php','/wp-load.php']
            for i in list:
                r = requests.get(f"{url}/{i}").status_code
                if(r == 200 or r == 403):
                    return True
            return False
        def info(url:str):
           response = requests.get(url)

           #X-AspNet-Version Server   X-Powered-By  Content Security Policy
           list = ['X-Powered-By','Server','X-AspNet-Version','Content-Security-Policy']
           headers_found = []
           number_header_found = []
           i = 0
           for header in list:
               try:
                   headers_found.append(response.headers[header])
                   number_header_found.append(list[i])
                   i+=1
               except: 
                   i+=1
                   pass
           if(len(headers_found)>0):
               return headers_found,number_header_found
           return "not found"
    

class filter():
    def filter_ips(file:str):
        try:
            file = open(file,'r',errors='ignore',encoding="utf8").read()
            ips = findall('[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}',file)
            return ips
        except:
            return "error in your file"
    def filter_unique_ip(array):
        if(type(array) == list):
            ips_found = []
            for i in array:
                if(i not in ips_found):
                    ips_found.append(i)
            return ips_found
        else:
            array = filter.filter_ips(array)
            ips_found = []
            for i in array:
                if(i not in ips_found):
                    ips_found.append(i)
            return ips_found
    def count_ips(array):
        if(type(array) == list):
            ips_count = {'ip':[],'number':[]}
            ips = []
            numbers = []
    
            for i in array:
                if(i not in ips):
                    ips.append(i)
                    number = 0
                    for ip in array:
                        if(ip == i):
                            number +=1
                    numbers.append(number)
            return ips,numbers
        else:
            all_ips = filter.filter_ips(array)
            ips_count = {'ip':[],'number':[]}
            ips = []
            numbers = []

            for i in all_ips:
                if(i not in ips):
                    ips.append(i)
                    number = 0
                    for ip in all_ips:
                        if(ip == i):
                            number +=1
                    numbers.append(number)
            return ips,numbers
    def urls(file:str):
        urls_list = []
        with open(file) as f:
            for line in f:
                urls = findall('https?://(?:[-\w.]|(?:%[\da-fA-F]{2}))+', line)
                if(urls):
                    for u in urls:
                        urls_list.append(u)
        return urls_list        

class network():
    
    class scan(): 
        
        def scanport_range(IP:str,PORT:int,PORT2:int):
            PORT = int(PORT)
            try: 
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                socket.setdefaulttimeout(1)
                status = {"port":[],"status":[]}
                while PORT <= PORT2 :
                    result = s.connect_ex((IP,PORT))
                    if result ==0:
                        #s.close()
                        status['port'].append("open")
                        status['status'].append(PORT)
                    else:
                        #s.close()
                        status['port'].append('close')
                        status['status'].append(PORT)
                    PORT+=1
                return status
            except:
                return "error try again please..."
        def scanport(IP:str,PORT:int):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                socket.setdefaulttimeout(1)
                result = s.connect_ex((IP,int(PORT)))
                if result ==0:
                    s.close()
                    return True
                else:
                    s.close()
                    return False
            except:
                return "error try again please..."
        def scanport_version(IP:str,PORT:int):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((IP, PORT))
                data = s.recv(1024)
                data = data.decode("utf-8")
                return data
            except:
                return "port closed"
    
    
    def ssh_exec(IP:str,username:str,password:str,commands:list):

        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        output = []
        command_enter = []
        try:
            client.connect(hostname=IP, username=username, password=password)
            for command in commands:
                command_enter.append("="*30 + command  + "="*30 )
                stdin, stdout, stderr = client.exec_command(command)
                output.append(stdout.read().decode())
                err = stderr.read().decode()
                if err:
                    print(err)
            return output,command_enter
        except:
            print("[!] Cannot connect to the SSH Server")
            exit()
    
    def ftp_upload(IP:str,username:str,password:str,file_name:str):
        ftp = ftplib.FTP(IP, username, password)
        # force UTF-8 encoding
        ftp.encoding = "utf-8"
        with open(file_name, "rb") as file:
            # use FTP's STOR command to upload the file
            ftp.storbinary(f"STOR {file_name}", file)
        ftp.quit()
        return "the file was uploaded"
    
    def ftp_download(IP:str,username:str,password:str,file_name:str):
        ftp_server = ftplib.FTP(IP, username, password)
        ftp_server.encoding = "utf-8"
        with open(file_name, "wb") as file:
            # Command for Downloading the file "RETR filename"
            ftp_server.retrbinary(f"RETR {file_name}", file.write)
        ftp_server.quit()
        return "the file was downloaded"
    
    def download_all_files_ftp(IP:str,username:str,password:str):

        ftp = ftplib.FTP(IP)
        ftp.login(username,password)

        files = ftp.nlst()


        try:
            for file in files:
                ftp.retrbinary("RETR " + file ,open(file, 'wb').write)
            ftp.close()
            return True
        except:
            pass
        return False
    
    class server:
        def __init__(self):
            self.bytes = 1024
            self.conn = ""
            self.blackips = []
            self.whiteips = []
        def listen(self,ip:str,port:int=1234):
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.bind((ip, port))
            self.s = s
            s.listen()
            
        def set_blacklist_ips(self,ips:list):
            self.blackips = ips
        
        def set_whitelist_ips(self,ips:list):
            self.whiteips = ips
        
        def accept(self):
            conn, addr = self.s.accept()
            self.addr = addr
            self.conn = conn
        def recv(self,bytes:int=1024): # recive data
            
            
            if(not self.whiteips): #  check blacklist ips
                
                if(self.addr[0] in self.blackips):
                    self.s.close()
                    self.accept = False
                    return self.addr[0],"blacklist"
                else:
                    return self.addr,self.conn.recv(bytes)
            else: # check if in whitelist
                if(self.addr[0] not in self.whiteips):
                    self.s.close()
                    return self.addr[0],"Not in white list"
                else:
                    return self.addr,self.conn.recv(bytes)
        def send(self,data:str): # send data
            if(self.accept):
                self.conn.send(data.encode()+"\r\n".encode())
        def exit(self):
            self.s.close()        
        def recvfile(self,file:str="file.txt"):
            f = open(file,'wb') #open in binary
            l = 1
            while(l):
                l = self.conn.recv(1024)
                while (l):
                    f.write(l)
                    l = self.conn.recv(1024)
                f.close()
    class clinet:
        def __init__(self):
            self.s = ""
        def connect(self,ip:str,port:int):
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s = s
            print(ip,port)
            s.connect((ip, port))
        def send(self,data):
            self.s.sendall(data.encode()+"\r\n".encode())
        def recv(self,bytes:int=1024):
            return  self.s.recv(bytes)
        def exit(self):
            self.s.close()
        def send_file(self,file):
            f = open (file, "rb")
            l = f.read(1024)
            while (l):
                self.s.send(l)
                l = f.read(1024)

class chart:
        class pie:
            def __init__(self):
                self.colors = ["#1f77b4","#ff7f0e","#2ca02c","#d62728"] 
                self.title = ""
            def set_data(self,data,data2):
              self.data = data
              self.data2 = data2 
            def show(self):
                fig = plt.figure()
                ax = fig.add_axes([0,0,1,1])
                ax.axis('equal')
                ax.pie(self.data2, labels = self.data,autopct='%1.2f%%',colors=self.colors)
                plt.show()
            def save(self,Filename:str="png.png"):
                  fig = plt.figure()
                  ax = fig.add_axes([0,0,1,1])
                  ax.axis('equal')
                  ax.pie(self.data2, labels = self.data,autopct='%1.2f%%',colors=self.colors)
                  plt.savefig(Filename)
            def set_colors(self,colors:list):
                  self.colors = colors
            def set_title(self,title:str=""):
                self.title = title

        class bar:
            def __init__(self):
                  self.xlabel_value = ""
                  self.title_value = ""
                  self.ylabel_value = ""
                  self.color="#17becf"
            def set_data(self,data:list,data2:list):
                self.data = data
                self.data2 = data2
            def show(self):
                plt.bar(self.data, self.data2,width = 0.4,color=self.color)
                plt.ylabel(self.ylabel_value)
                plt.xlabel(self.xlabel_value)
                plt.title(self.title_value)
                plt.show()
            def set_ylabel(self,label:str=""):
                  self.ylabel_value = label
            def set_xlabel(self,label:str=""):
                  self.xlabel_value = label
            def set_title(self,label:str=""):
                  self.title_value = label
            def set_color(self,color:str):
                  self.color = color
            def save(self,Filename:str="png.png"):
                plt.bar(self.data, self.data2,width = 0.4,color=self.color)
                plt.ylabel(self.ylabel_value)
                plt.xlabel(self.xlabel_value)
                plt.title(self.title_value)
                plt.savefig(Filename)

