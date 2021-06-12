import pygame
import time
import requests
from selenium import webdriver
# from selenium.webdriver.common.by import By

# the configuration
ip_url = "http://192.168.43.1:8080/"
driver = webdriver.Chrome()
driver.get(ip_url)

# the function that used to warn
def yell():
    pygame.init()

    music=pygame.mixer.Sound("louder.mp3") 
    music.play()

    timeout = time.time() + 2.5
    while True:
        test = 0
        if time.time() > timeout:
            break
        
    pygame.quit()

# get the data from the website
def get_data():
    
    views = driver.find_element_by_xpath('//*[@id="element1"]/span[2]/span[1]')
    number = (float(views.text))
    print(number)

    return bool(number > 100)

# main function
def main():
    while True:
        # get the data from the website
        is_loud = get_data()

        if is_loud==True:
            yell()

if __name__ == '__main__':
    main()