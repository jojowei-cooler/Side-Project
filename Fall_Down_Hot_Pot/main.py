import pygame
import time
import requests
from bs4 import BeautifulSoup

def yell():
    pygame.init()

    music=pygame.mixer.Sound("hotpot.mp3") 
    music.play()

    timeout = time.time() + 6
    while True:
        test = 0
        if time.time() > timeout:
            break
        
    pygame.quit()

def get_data():
    ip_url = "http://192.168.43.1:8080/"
    headers={
        "Content-Type": "application/json",
        "User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.102 Safari/537.36"
    }

    rs = requests.Session()
    response = rs.get(ip_url, headers=headers)
    doc = BeautifulSoup(response.text, "html.parser")
    s1 = doc.find("span", attrs={"class": "valueNumber"})
    s2 = doc.find_all("span")
    print(s2)

    acc_data = doc.find_all("span", class_="valueNumber")
    for acc in acc_data:
        if acc !=None:
            print(acc.string)

    return bool(5<0)

def main():
    while True:
        # get the data from the website
        is_fall = get_data()
        
        time.sleep(5)
        # exit(1)

        if is_fall==True:
            yell()

if __name__ == '__main__':
    main()