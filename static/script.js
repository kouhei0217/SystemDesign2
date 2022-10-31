// HTMLを読み込み終わったら
window.addEventListener("DOMContentLoaded", function () {
  // 10秒毎に実行
  setInterval(() => {
    // ページをリロード
    window.location.reload();
    fetch("http://127.0.0.1/twitter", {
      method: "GET",
      headers: { "Content-Type": "application/json" },
    })
      .then(function (response) {
        return response.json();
      })
      .then(function (data) {
        document.getElementById("image").src = base64.decode(data["image"]);
      });
  }, 10000);
});
