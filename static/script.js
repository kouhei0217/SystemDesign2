// window.addEventListener("DOMContentLoaded", function () {
//   setInterval(() => {
//     // window.location.reload();
//   }, 10000);
// });

FetchMenus();

if (document.cookie != "") {
  console.log("a");
  document.getElementsByName("menus").disable = true;
  document.getElementById("vote_menu").disable = true;
  for (let cookieList of document.cookies.split(";")) {
    cookie = cookieList.split("=");
    if (cookie[0] == "menu_id") {
      document.getElementById(cookie[1]).checked = true;
    }
  }
}
