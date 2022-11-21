import base64

from flask import Flask, Response, render_template, request
from flask_cors import CORS

from python.HandleDatabase import (AddMenu, FetchImage, FetchMenu, FetchMenus,
                                   SaveImage, VoteMenu)

app = Flask(__name__)
app.config["MAX_CONTENT_LENGTH"] = 100 * 1024 * 1024
CORS(
    app,
    supports_credentials=True
)


@app.route("/", methods=["GET"])
def IndexHTML():
    return render_template("index.html", image_id=FetchImage()[0]["image_id"])


@app.route("/admin", methods=["GET"])
def AdminHTML():
    return render_template("admin.html")


@app.route("/fetch-image", methods=["GET"])
def CallFetchImage():
    with open("static/images/" + str(FetchImage()[0]["image_id"]) + ".jpg", "rb") as file:
        imageBase64 = base64.b64encode(file.read()).decode("utf-8")
    return {"image": imageBase64}


@ app.route("/save-image", methods=["POST"])
def CallSaveImage():
    imagePath = "./static/images/" + \
        str(FetchImage()[0]["image_id"] + 1) + ".jpg"
    with open(imagePath, mode="wb") as file:
        file.write(base64.b64decode(request.json["image"]))
    SaveImage()
    return Response(status=204)


@ app.route("/fetch-menu", methods=["GET"])
def CallFetchMenu():
    return {"menu_name": FetchMenu()}


@ app.route("/fetch-menus", methods=["GET"])
def CallFetchMenus():
    return {"menus": FetchMenus()}


@ app.route("/vote-menu", methods=["POST"])
def CallVoteMenu():
    VoteMenu(request.json["menu_id"])
    return Response(status=204)


@ app.route("/add-menu", methods=["POST"])
def CallAddMenu():
    AddMenu(request.json["menu_name"])
    return Response(status=204)


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8080, debug=True)
