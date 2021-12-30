const url = 'http://127.0.0.1:5000/user';
function validate() {
    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;

    var datastring = `${url}?username=${username}&password=${password}&eggId=${username + "1"}`

    const response = new XMLHttpRequest();
    response.open("GET", datastring)


    response.send();
    response.onload = (e) => {
        console.log(response);
    }


}
