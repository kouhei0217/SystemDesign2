let date = new Date();
FetchMenu();
FetchMenus();

function CheckCookie() {
  if (document.cookie != "") {
    elements = document.getElementsByName("menus");
    for (let i = 0; i < elements.length; i++) {
      elements.item(i).disabled = true;
    }
    document.getElementById("vote_menu").disabled = true;
    document.getElementById("vote_menu").textContent =
      date.getMonth() + 1 + "月分の投票完了";
    document.getElementById(document.cookie.split("=")[1]).checked = true;
  }
}
