FetchMenus();
checkCookie();

function checkCookie() {
  if (document.cookie != "") {
    console.log("a");
    document.getElementsByName("menus").disable = true;
    document.getElementById("vote_menu").disable = true;
    cookie = document.cookie.split("=");
    if (cookie[0] == "menu_id") {
      document.getElementById(cookie[1]).checked = true;
    }
  }
}
