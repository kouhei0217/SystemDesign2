FetchMenus();
checkCookie();

function checkCookie() {
  if (document.cookie != "") {
    document.getElementsByName("menus").disabled = true;
    document.getElementById("vote_menu").disabled = true;
    cookie = document.cookie.split("=");
    if (cookie[0] == "menu_id") {
      console.log("a");
      document.getElementById(cookie[1]).checked = true;
    }
  }
}
