import paho.mqtt.client as mqtt
from datetime import datetime
from sqlalchemy import create_engine, Column, Integer, Float, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
import os
import time
POSTGRES_USER = os.getenv("POSTGRES_USER")
POSTGRES_PASSWORD = os.getenv("POSTGRES_PASSWORD")
POSTGRES_DB = os.getenv("POSTGRES_DB")
POSTGRES_HOST = os.getenv("POSTGRES_HOST")
POSTGRES_PORT = os.getenv("POSTGRES_PORT")
MQTT_BROKER_HOST = os.getenv("MQTT_BROKER_HOST")
DATABASE_URL = f"postgresql+psycopg2://{POSTGRES_USER}:{POSTGRES_PASSWORD}@{POSTGRES_HOST}:{POSTGRES_PORT}/{POSTGRES_DB}"

Base = declarative_base()

class SensorData(Base):
    __tablename__ = "sensor_data"
    id = Column(Integer, primary_key=True)
    timestamp = Column(DateTime, default=datetime.utcnow)
    temperature = Column(Float)
    humidity = Column(Float)

engine = create_engine(DATABASE_URL)
Base.metadata.create_all(engine)
Session = sessionmaker(bind=engine)

session = Session()
temp_value = None
humidity_value = None

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("pic2/temperature")
    client.subscribe("pic2/humidity")

def on_message(client, userdata, msg):
    global temp_value, humidity_value
    print(msg.topic + " " + str(msg.payload))
    value=float(msg.payload)
    if msg.topic == "pic2/temperature":
        temp_value = value

    elif msg.topic == "pic2/humidity":
        humidity_value = value

    if temp_value is not None and humidity_value is not None:
        session.add(SensorData(temperature=temp_value,humidity=humidity_value))
        session.commit()
        temp_value = None
        humidity_value = None



client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER_HOST, 1883, 60)
client.loop_forever()