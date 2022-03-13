import json
import requests
# api-endpoint
URL = "http://127.0.0.1:8000/egg"
LOGINURL = "http://127.0.0.1:8000/user?username=user&password=user"
EGGURL = "http://127.0.0.1:8000/egg?eggId=es664168431&datatype=ph&username=user"

with open("sample.csv","r") as f:
    filedata = f.read()
  
# defining a params dict for the parameters to be sent to the API
PARAMS = {"data":filedata,"username":"user"}

USERPARAMS = {
        "username": "user",
        "password": "user",
        "eggId":"",
        "email": "user@gmail.com"
    }
# sending get request and saving the response as response object
#r = requests.post(url = URL, json=PARAMS)
r = requests.get(url = EGGURL)
# extracting data in json format
data = r.json()
  
print(data)