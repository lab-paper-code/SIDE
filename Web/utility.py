import os
import configparser

from datetime import datetime
from pymongo import MongoClient


def read_config(config_path):
    config = configparser.ConfigParser()
    if os.path.exists(config_path):
        config.read(config_path)
        return config
    else:
        raise FileNotFoundError(f"Configuration file not found: {config_path}")
    
def input_parser(form):
    json_data = {}
    diagram_type_map = {
        "CIRCLE": 0,
        "RECTANGLE": 1,
        "POLYGON": 2
    }
    diagram_type = diagram_type_map.get(form['searchType'], -1)

    start_datetime = datetime.strptime(f"{form['start_date']} {form['start_hour']}", "%Y-%m-%d %H")
    end_datetime = datetime.strptime(f"{form['end_date']} {form['end_hour']}", "%Y-%m-%d %H")

    time_data = [
        start_datetime.year, start_datetime.month, start_datetime.day, start_datetime.hour,
        end_datetime.year, end_datetime.month, end_datetime.day, end_datetime.hour
    ]

    spatial_data = []
    if diagram_type == 1:  # RECTANGLE
        points_raw = form['points']
        spatial_data = [
            float(coord) for point in points_raw.split("), (")
            for coord in point.strip(" ()").split(", ")
        ]

        json_data = {
            "type": diagram_type,
            "coordinates": spatial_data,
            "time": time_data
        }
    else:
        raise ValueError("Invalid Diagram Type")
    
    return json_data

def extract_coordinates_from_documents(documents):
    """
    Extracts [lng, lat] coordinate pairs from MongoDB documents
    for use in frontend map rendering.

    Parameters:
        documents (list of dict): List of MongoDB documents

    Returns:
        List[dict]: List of {'lat': float, 'lng': float}
    """
    coordinates = []
    for doc in documents:
        try:
            lon, lat = doc["location"]["coordinates"]
            coordinates.append({"lat": lat, "lng": lon})
        except Exception as e:
            print(f"[Error] Skipping doc due to: {e}")
            continue
    return coordinates

def fetch_contextual_points(db_config, bounding_box, padding_ratio=0.05):
    """
    Fetches nearby points slightly outside the user-defined bounding box 
    for contextual visualization using MongoDB's $geoWithin and GeoJSON Polygon.

    Parameters:
        db_config (dict): MongoDB connection config
        bounding_box (list): [lat1, lng1, lat2, lng2]
        padding_ratio (float): Ratio to expand the box (default 5%)

    Returns:
        List[dict]: List of MongoDB documents in the expanded polygon range
    """
    db_info = db_config['DATABASE']
    user = db_info['ID']
    password = db_info['PASSWD']
    ip = db_info['IP']
    port = db_info['PORT']
    db_name = db_info['DB']
    collection_name = db_info['COLLECTION']

    uri = f"mongodb://{user}:{password}@{ip}:{port}"

    lat1, lng1, lat2, lng2 = bounding_box
    min_lat, max_lat = min(lat1, lat2), max(lat1, lat2)
    min_lng, max_lng = min(lng1, lng2), max(lng1, lng2)

    lat_pad = (max_lat - min_lat) * padding_ratio
    lng_pad = (max_lng - min_lng) * padding_ratio

    expanded_min_lat = min_lat - lat_pad
    expanded_max_lat = max_lat + lat_pad
    expanded_min_lng = min_lng - lng_pad
    expanded_max_lng = max_lng + lng_pad

    geo_box = {
        "type": "Polygon",
        "coordinates": [[
            [expanded_min_lng, expanded_min_lat],
            [expanded_max_lng, expanded_min_lat],
            [expanded_max_lng, expanded_max_lat],
            [expanded_min_lng, expanded_max_lat],
            [expanded_min_lng, expanded_min_lat]
        ]]
    }

    client = MongoClient(uri)
    db = client[db_name]
    collection = db[collection_name]

    index_info = collection.index_information()
    has_2dsphere_index = any(
        idx['key'][0][0] == 'location' and idx['key'][0][1] == '2dsphere'
        for idx in index_info.values()
    )

    if not has_2dsphere_index:
        collection.create_index([("location", "2dsphere")], name="2dsphere")

    query = {
        "location": {
            "$geoWithin": {
                "$geometry": geo_box
            }
        }
    }

    documents = list(collection.find(query))
    client.close()
    return documents