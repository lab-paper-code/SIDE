import os

import queryProcessor
import utility

from flask import Flask, render_template, request
from time import time

app = Flask(__name__)
app.config['DEBUG'] = True

visual = True
index_config_parser = None
web_config_parser = None

# Home Page Rendering
@app.route('/', methods=['GET', 'POST'])
@app.route('/main', methods=['GET', 'POST'])
def main():
    return render_template(
        'main.html',
        title = 'SIDE'
    )

# Render Search Tab
@app.route('/search')

# Point Range Query - Search
@app.route('/search/range', methods=['GET', 'POST'])
def range_query():
    return render_template(
        'search_range.html',
        country=web_config_parser['WEB_SERVER']["VIEW"]
    )

# Point Range Query - Result
@app.route('/result/range', methods=['GET', 'POST'])
def result_range():
    if request.method == 'POST':
        query_data = utility.input_parser(request.form)
    print('user input: ', query_data)

    start_time = time()
    res = queryProcessor.point_range_query(query_data, index_config_parser)
    iot_data = queryProcessor.data_from_database(res, index_config_parser)
    end_time = time()
    elapsed_time = round(end_time - start_time, 4)

    if(visual):
        raw_data = utility.fetch_contextual_points(index_config_parser, query_data['coordinates'])
        raw_coords = utility.extract_coordinates_from_documents(raw_data)

        coords = utility.extract_coordinates_from_documents(iot_data)
    else:
        raw_coords = None
        coords = None

    print("# of searched data: {}".format(len(res['_id'])))
    print("total elapesd time (s): {}\n".format(elapsed_time))

    return render_template(
        'result_range.html',
        res=res,
        elapsed_time=elapsed_time,
        raw_coords=raw_coords,
        coords=coords,
        spatial_range=query_data['coordinates']
    )


def main():
    global index_config_parser, web_config_parser
    index_config_path = os.path.join(os.getcwd(), "Config", "index.ini")
    web_config_path = os.path.join(os.getcwd(), "Config", "web.ini")
    index_config_parser = utility.read_config(index_config_path)
    web_config_parser = utility.read_config(web_config_path)

    web_ip = web_config_parser['WEB_SERVER']['IP']
    web_port = int(web_config_parser['WEB_SERVER']['PORT'])

    app.run(debug=True, threaded=True, host=web_ip, port=web_port)
    app.jinja_env.auto_reload = True


if __name__ == '__main__':
    main() 