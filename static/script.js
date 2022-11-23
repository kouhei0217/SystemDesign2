window.addEventListener("DOMContentLoaded", function () {
  setInterval(() => {
    // window.location.reload();
  }, 10000);
});

FetchMenus();

for (let cookieList of document.cookies.split(";")) {
  cookie = cookieList.split("=");
  if (cookie[0] == "has_voted") {
    if (cookie[1] == 1) {
      document.getElementsByName("menus").disable = true;
      document.getElementById("vote_menu").disable = true;
    }
  } else {
    break;
  }
  if (cookie[0] == "menu_id") {
    document.getElementById(cookie[1]).checked = true;
  }
}
