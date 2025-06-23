import json

from requests import post
from pymongo import MongoClient
from bson.objectid import ObjectId


def point_range_query(query_data, index_config):
    '''
    :param
    request: {
        'type' : 0 == circle, 1 == rectangle, 2 == polygon
        'radius': float (unit: km) if type == 0
        'time': [YY, MM, DD, HH, YY, MM, DD, HH] (start, end)
        'coordinates': [lat, lng, lat, lng, ... ]
    }
    :return:
        list of Object_ID included in spatiotemporal range
    '''
    INDEX_IP = index_config['INDEX_SERVER']['IP']
    INDEX_PORT = index_config['INDEX_SERVER']['PORT']
    INDEX_URL = f"http://{INDEX_IP}:{INDEX_PORT}"

    # parsing
    request=dict()
    request['type'] = query_data['type']
    if request['type'] == 0:
        request['radius'] = query_data['radius']
    request['time'] = query_data['time']
    request['coordinates'] = query_data['coordinates']
    
    # hand over data to index server
    header = {'Content-Type': 'application/json; charset=utf-8'}
    try:
        res = post(INDEX_URL, headers=header, data=json.dumps(request),timeout=6000)
    except ConnectionError as e:
        print("Connection Error: ", e)
        return []
    
    return res.json()


def data_from_database(res_id, db_config):
    """
        Although this function is currently implemented using MongoDB, 
        it is designed to be easily customizable for other database systems.
        By following the same db_config structure, you can switch to databases 
        such as PostgreSQL, MySQL, or others by replacing the connection and query logic accordingly.
    """
    chunk_size = 10000

    db_info = db_config['DATABASE']
    user = db_info['ID']
    password = db_info['PASSWD']
    ip = db_info['IP']
    port = db_info['PORT']
    db_name = db_info['DB']
    collection_name = db_info['COLLECTION']

    uri = f"mongodb://{user}:{password}@{ip}:{port}"

    # Connect to Database
    client = MongoClient(uri)
    db = client[db_name]
    collection = db[collection_name]

    # Convert to ObjectId list
    object_ids = [ObjectId(_id) for _id in res_id['_id']]
    all_docs = []
    for i in range(0, len(object_ids), chunk_size):
        chunk = object_ids[i:i + chunk_size]
        query = {"_id": {"$in": chunk}}
        cursor = collection.find(query)

        for doc in cursor:
            all_docs.append(doc)

    client.close()

    return all_docs