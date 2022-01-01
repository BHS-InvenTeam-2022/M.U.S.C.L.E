const url = 'http://127.0.0.1:5000/user';
function validate() {
    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;

    const response = new XMLHttpRequest();

    const data = JSON.stringify({
        "username": username,
        "password": password,
        "eggId":username+"1",
        "eggnames":"myprecious"

    }
    );

    response.open("POST", url)
    response.setRequestHeader('Content-Type', 'application/json');

    response.send(data);

    response.onload = (e) => {
        console.log(response.response);
        window.location.href = "./login.html";
    }


}

