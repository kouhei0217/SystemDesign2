FetchMenus();
checkCookie();

function checkCookie() {
  if (document.cookie != "") {
    elements = document.getElementsByName("menus").disabled = true;
    for (let i = 0; i < elements.length; i++) {
      elements.item(i).disabled = true;
    }
    document.getElementById("vote_menu").disabled = true;
    cookie = document.cookie.split("=");
    if (cookie[0] == "menu_id") {
      console.log("a");
      document.getElementById(cookie[1]).checked = true;
    }
  }
}
