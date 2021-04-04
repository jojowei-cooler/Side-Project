from selenium import webdriver
import time
# from selenium.webdriver.common.by import By

ip_url = "http://192.168.43.1:8080/"
driver = webdriver.Chrome()
driver.get(ip_url)
while True:
    views = driver.find_elements_by_xpath("/html/body/div[3]/div[3]/div/span[2]/span[1]")
    print(views)
    time.sleep(5)