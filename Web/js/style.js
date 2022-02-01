const toggleButton = document.getElementsByClassName('toggle-button')[0];
const navbarLinks = document.getElementsByClassName('navbar-links')[0];

toggleButton.addEventListener('click',()=>{
    navbarLinks.classList.toggle('active');
});

window.onload = function userRegister() {
    const loginbtn = document.getElementById("login-btn").firstChild;
    if(sessionStorage){
        if(sessionStorage.getItem("Username")){
            loginbtn.textContent = sessionStorage.getItem("Username");
            loginbtn.href = "javascript:void(0)";
        }
    }
}