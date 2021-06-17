import os
import pyzipper

# change to the working directory first
os.chdir("C:\\Users\\jojowei\\Desktop\\SideProject\\Conquer\\zip_breaker")

count = 1 # counter in dictionary
with open('from_darkweb1000.txt', 'rb') as text: # open file with binary access
    for entry in text.readlines():
        password = entry.strip() # default is space
        
        try: # for CRC32
            with pyzipper.ZipFile('cypher.zip', 'r') as zf:
                zf.extractall(pwd= password)
                data = zf.namelist()[0]
                data_size = zf.getinfo(data).file_size
                print("------------------------------------------------")
                print("\npassword found by CRC!\npassword: %s\ndata: %s\ndata size: %s\n" %(password.decode('utf8'), data, data_size))
                print("------------------------------------------------")
                break
        except:
            pass
        
        try: # for AES
            with pyzipper.AESZipFile('cypher.zip', 'r') as zf:
                zf.extractall(pwd= password)
                data = zf.namelist()[0]
                data_size = zf.getinfo(data).file_size
                print("------------------------------------------------")
                print("\npassword found by AES!\npassword: %s\ndata: %s\ndata size: %s\n" %(password.decode('utf8'), data, data_size))
                print("------------------------------------------------")
                break
        except:
            number = count
            print("%s: %s >> password failure" %(number, password.decode('utf8')))
            count += 1