from flask import Flask, request
from flask_restful import Api, Resource, reqparse, abort, fields, marshal_with
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import json
from datetime import datetime

app = Flask(__name__)
api = Api(app)
CORS(app, resources={r"*": {"origins": "*"}})
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///database.db"
app.config["SQLALCHEMY_BINDS"] = {"data": "sqlite:///database2.db"}


db = SQLAlchemy(app)


class UserModel(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(50), nullable=False, unique=True)
    password = db.Column(db.String(50), nullable=False)
    eggId = db.Column(db.String(50), nullable=False)
    eggnames = db.Column(db.String(250), nullable=False)

    def __repr__(self):
        return f"User(username={UserModel.username}, password={UserModel.password}, eggId={UserModel.eggId}, eggnames={UserModel.eggnames})"


class EggModel(db.Model):
    __bind_key__ = "data"
    id = db.Column(db.Integer, primary_key=True)
    eggId = db.Column(db.String(50), nullable=False)
    temp_id = db.Column(db.Integer, nullable=False)
    temp_data = db.Column(db.Float, nullable=False)
    ph_id = db.Column(db.Integer, nullable=False)
    ph_data = db.Column(db.Float, nullable=False)
    clock_id = db.Column(db.Integer, nullable=False)
    clock_data = db.Column(db.String(25), nullable=False)
    salinity_id = db.Column(db.Integer, nullable=False)
    salinity_data = db.Column(db.Float, nullable=False)
    data_packet = db.Column(db.String, nullable=False)
    clock_packet = db.Column(db.String, nullable=False)

    def __repr__(self):
        return f"Egg Data(eggId={EggModel.eggId}, temp={EggModel.temp_data}, ph={EggModel.ph_data}, time={EggModel.clock_data}, salinity={EggModel.salinity_data})"


db.create_all()  # only done once so once initialized remove


user_post_args = reqparse.RequestParser()
user_post_args.add_argument(
    "username", type=str, help="Username is required", required=True
)
user_post_args.add_argument(
    "password", type=str, help="Password is required", required=True
)
user_post_args.add_argument("eggId", type=str, help="Egg ID is required", required=True)
user_post_args.add_argument(
    "eggnames", type=str, help="Egg Name is required", required=True
)

egg_post_args = reqparse.RequestParser()
egg_post_args.add_argument("eggId", type=str, help="EggId is required", required=True)
egg_post_args.add_argument(
    "temp_id", type=int, help="temp ID is required", required=True
)
egg_post_args.add_argument(
    "temp_data", type=float, help="temp data is required", required=True
)
egg_post_args.add_argument("ph_id", type=int, help="ph ID is required", required=True)
egg_post_args.add_argument(
    "ph_data", type=float, help="ph Data is required", required=True
)
egg_post_args.add_argument(
    "clock_id", type=int, help="clock ID is required", required=True
)
egg_post_args.add_argument(
    "clock_data", type=str, help="clock data is required", required=True
)
egg_post_args.add_argument(
    "salinity_id", type=int, help="salinity ID is required", required=True
)
egg_post_args.add_argument(
    "salinity_data", type=float, help="salinity data is required", required=True
)


user_resource_fields = {
    "id": fields.Integer,
    "username": fields.String,
    "password": fields.String,
    "eggId": fields.String,
    "eggnames": fields.String,
}

egg_resource_fields = {
    "id": fields.Integer,
    "eggId": fields.String,
    "temp_id": fields.Integer,
    "temp_data": fields.Float,
    "ph_id": fields.Integer,
    "ph_data": fields.Float,
    "clock_id": fields.Integer,
    "clock_data": fields.String,
    "salinity_id": fields.Integer,
    "salinity_data": fields.Float,
    "data_packet": fields.String,
    "clock_packet": fields.String,
}


def check_valid(*argv):
    for arg in argv:
        if arg == "":
            return False
    return True

def g_list(data):
    chars = []
    for x in data:
        chars.append(x)
        if chars[-1]==";":
            yield ''.join(chars[:-1])
            chars = []
    yield ''.join(chars)

class User(Resource):
    @marshal_with(user_resource_fields)
    def get(self):
        args = request.args
        result = UserModel.query.filter_by(username=args["username"]).first()
        if not result:
            abort(404, message="Could not find user...")
        if not args["password"] == result.password:
            abort(409, message="Incorrect password...")

        return result, 200

    @marshal_with(user_resource_fields)
    def post(self):
        # neaargs = request.data
        args = user_post_args.parse_args()
        result = UserModel.query.filter_by(username=args["username"]).first()
        if result:
            abort(409, message="username already taken")
        newuser = UserModel(
            username=args["username"],
            password=args["password"],
            eggId=args["eggId"],
            eggnames=args["eggnames"],
        )
        db.session.add(newuser)
        db.session.commit()
        return newuser, 201

    @marshal_with(user_resource_fields)
    def put(self):
        return "", 404

    @marshal_with(user_resource_fields)
    def patch(self):
        return "", 404

    @marshal_with(user_resource_fields)
    def delete(self):
        args = user_post_args.parse_args()
        UserModel.query.filter_by(username=args["username"]).delete()
        db.session.commit()
        return "", 204


class Egg(Resource):
    @marshal_with(egg_resource_fields)
    def get(self):
        args = request.args
        data_result = []
        clock_result = []
        if args["datatype"] == "temp":
            data_result = [
                egg.temp_data for egg in EggModel.query.filter_by(eggId=args["eggId"])
            ]
            clock_result = [
                egg.clock_data for egg in EggModel.query.filter_by(eggId=args["eggId"])
            ]
        elif args["datatype"] == "ph":
            data_result = [
                egg.ph_data for egg in EggModel.query.filter_by(eggId=args["eggId"])
            ]
            clock_result = [
                egg.clock_data for egg in EggModel.query.filter_by(eggId=args["eggId"])
            ]
        elif args["datatype"] == "salinity":
            data_result = [
                egg.salinity_data
                for egg in EggModel.query.filter_by(eggId=args["eggId"])
            ]
            clock_result = [
                egg.clock_data for egg in EggModel.query.filter_by(eggId=args["eggId"])
            ]

        newpoint = EggModel(data_packet=data_result, clock_packet=clock_result)
        return newpoint, 200

    @marshal_with(egg_resource_fields)
    def post(self):
        args = request.data
        args = json.loads(args)

        if ";" not in args["clock_data"]:
            if not check_valid(
                args["temp_data"],
                args["ph_data"],
                args["clock_data"],
                args["salinity_data"],
            ):
                return "", 201
            result = EggModel.query.filter_by(
                eggId=args["eggId"],
                clock_data=args["clock_data"].strip("\r")
            ).first()
            if not result:
                newpoint = EggModel(
                    eggId=args["eggId"],
                    temp_id=args["temp_id"],
                    temp_data=args["temp_data"],
                    ph_id=args["ph_id"],
                    ph_data=args["ph_data"],
                    clock_id=args["clock_id"],
                    clock_data=args["clock_data"].strip("\r"),
                    salinity_id=args["salinity_id"],
                    salinity_data=args["salinity_data"],
                    data_packet="",
                    clock_packet="",
                )
                db.session.add(newpoint)
                db.session.commit()
                return "", 201
            else:
                return "", 201


        clock_data = g_list(args["clock_data"])
        len_data = len(list(clock_data))
        clock_data = g_list(args["clock_data"])
        temp_data = g_list(args["temp_data"])
        ph_data = g_list(args["ph_data"])
        salinity_data = g_list(args["salinity_data"])
        tmp_eggId = args["eggId"]

        index = 0
        if len(EggModel.query.filter_by(eggId=tmp_eggId,).all()) > 1:
            result = EggModel.query.filter_by(eggId=tmp_eggId).all()[-1]
            if result:
                for i in range(len_data):
                    index += 1
                    t_temp_data = next(temp_data)
                    t_ph_data = next(ph_data)
                    t_salinity_data = next(salinity_data)
                    if next(clock_data) > result.clock_data:
                        break

        len_data -= index        

        counter = 0
        then  = datetime.now()
        for i in range(len_data):
            t_clock_data = next(clock_data)
            t_temp_data = next(temp_data)
            t_ph_data = next(ph_data)
            t_salinity_data = next(salinity_data)

            if not check_valid(t_clock_data, t_temp_data, t_ph_data, t_salinity_data):
                continue

            newpoint = EggModel(
                eggId=tmp_eggId,
                temp_id=args["temp_id"],
                temp_data=t_temp_data,
                ph_id=args["ph_id"],
                ph_data=t_ph_data,
                clock_id=args["clock_id"],
                clock_data=datetime.fromisoformat(t_clock_data.strip("\r")),
                salinity_id=args["salinity_id"],
                salinity_data=t_salinity_data,
                data_packet="",
                clock_packet="",
            )
            db.session.add(newpoint)
            counter += 1
            if counter%10000 == 0:
                now  = datetime.now()#remove this timer later
                duration = now - then
                print(counter)
                print(duration.total_seconds())
                then = datetime.now()
                db.session.commit()

        db.session.commit()
        print(counter)
        return "", 201

    @marshal_with(egg_resource_fields)
    def put(self):
        return "", 404

    @marshal_with(egg_resource_fields)
    def patch(self):
        return "", 404

    @marshal_with(egg_resource_fields)
    def delete(self):
        return "", 204


api.add_resource(User, "/user", endpoint="user")
api.add_resource(Egg, "/egg", endpoint="egg")

if __name__ == "__main__":
    app.run(debug=True)
