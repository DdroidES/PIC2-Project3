FROM python:3.12-slim
WORKDIR /app
COPY . .
RUN pip install paho-mqtt sqlalchemy psycopg2-binary
CMD ["python", "mqtt_subscriber.py"]