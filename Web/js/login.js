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
        if(response.status == 200){
            serverresponse = JSON.parse(response.response);
            sessionStorage.setItem("username",serverresponse.username);
            sessionStorage.setItem("eggId",serverresponse.eggId);
            sessionStorage.setItem("eggnames",serverresponse.eggnames);
            window.location.href = "./chart.html";
        }else{
            alert(JSON.parse(response.response).message);
            return;
        }
    }


}
