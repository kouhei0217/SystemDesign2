import base64
import os

from flask import Flask, Response, render_template, request, url_for
from flask_cors import CORS

from python.HandleDatabase import (AddMenu, FetchImage, FetchMenu, FetchMenus,
                                   SaveImage, VoteMenu)

app = Flask(__name__)
app.config["MAX_CONTENT_LENGTH"] = 100 * 1024 * 1024
CORS(
    app,
    supports_credentials=True
)


@app.context_processor
def override_url_for():
    return dict(url_for=dated_url_for)


def dated_url_for(endpoint, **values):
    if endpoint == "static":
        filename = values.get("filename", None)
        if filename:
            file_path = os.path.join(app.root_path,
                                     endpoint, filename)
            values["q"] = int(os.stat(file_path).st_mtime)
    return url_for(endpoint, **values)


@app.route("/", methods=["GET"])
def WebAPI():
    return render_template("index.html")


@app.route("/fetch-image", methods=["GET"])
def FetchImageAPI():
    with open("static/" + FetchImage() + ".jpg", "rb") as file:
        imageBase64 = base64.b64encode(file.read()).decode("utf-8")
    return {"image": imageBase64}


@app.route("/save-image", methods=["POST"])
def SaveImageAPI():
    fs = request.files["image"]
    imagePath = "./static/" + (FetchImage()["image_id"] + 1) + ".jpg"
    fs.save(imagePath)
    SaveImage()
    return Response(status=204)


@app.route("/fetch-menu", methods=["GET"])
def FetchMenuAPI():
    return {"menu_name": FetchMenu()}


@app.route("/fetch-menus", methods=["GET"])
def FetchMenusAPI():
    return {"menus": FetchMenus()}


@app.route("/vote-menu", methods=["POST"])
def VoteMenuAPI():
    VoteMenu(request.json["menu_id"])
    return Response(status=204)


@app.route("/add-menu", methods=["POST"])
def AddMenuAPI():
    AddMenu(request.json["menu_name"])
    return Response(status=204)


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8080, debug=True)
