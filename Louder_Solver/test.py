from selenium import webdriver
import time
# from selenium.webdriver.common.by import By

ip_url = "http://192.168.43.1:8080/"
driver = webdriver.Chrome()
driver.get(ip_url)
while True:
    views = driver.find_element_by_xpath('//*[@id="element1"]/span[2]/span[1]')
    
    number = (float(views.text))
    print(number)
    time.sleep(5)
    