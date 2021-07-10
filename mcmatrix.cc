// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// By default, connects to the installation at Noisebridge. If using a
// different display (e.g. a local terminal display)
// pass the hostname as parameter:
//
//  ./mcmatrix localhost
//
// .. or set the environment variable FT_DISPLAY to not worry about it
//
//  export FT_DISPLAY=localhost
//  ./mcmatrix
//
//  macOS fix - Error sending packet.: Message too long
//  https://github.com/hzeller/flaschen-taschen/blob/master/client/README.md#udp-size

#include "udp-flaschen-taschen.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

// for convenience
using namespace std;
using json = nlohmann::json;

#define DISPLAY_WIDTH  64
#define DISPLAY_HEIGHT 64

int main(int argc, char *argv[]) {
    const char *hostname = NULL;   // will use default if not set otherwise
    if (argc > 1) {
        hostname = argv[1];        // hostname can be supplied as first arg
    }

    // open socket and create our canvas
    const int socket = OpenFlaschenTaschenSocket(hostname);
    UDPFlaschenTaschen canvas(socket, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // define matrix colors
    const Color red(255, 0, 0);         // broken
    const Color green(0, 255, 0);       // working
    const Color blue(0, 0, 255);
    const Color white(255, 255, 255);
    const Color gray(175, 175, 175);    // inactive
    const Color yellow(255, 255, 0);

    // download markers.json from live url
    // system("curl https://mcbroken2.nyc3.digitaloceanspaces.com/markers.json --compressed -o markers.json");

    // parse json
    ifstream ifs("markers.json");
    json markers = json::parse(ifs);
    json features = markers["features"];

    // don't truncate lat/lng
    cout.precision(8);

    cout << endl;
    // cout << "Number of McDonald's: " << features.size() << endl;
    int count = 0; 

    for (const auto& feature : features.items()) {
        json properties = feature.value()["properties"];

        string country = properties["country"].get<string>();
        if (country != "USA") continue;

        string state = properties["state"].get<string>();
        if (state != "MN") continue;

        json coordinates = feature.value()["geometry"]["coordinates"];
        double lat = stod(coordinates[1].get<string>());
        double lng = stod(coordinates[0].get<string>());

        // TODO: make getLocationForCoords
        double latMin = 44.716;
        double latMax = 45.250;
        // diff: 0.534

        double lngMin = -92.849;
        double lngMax = -93.607;
        // diff: 0.758

        // only include stores in region
        if (lat < latMin || lat > latMax) continue;
        if (lng > lngMin || lng < lngMax) continue;

        int hPoint = DISPLAY_WIDTH - DISPLAY_WIDTH * ((lngMin - lng) / (lngMin - lngMax));
        int vPoint = DISPLAY_WIDTH - DISPLAY_WIDTH * ((lat - latMin) / (latMax - latMin));

        string city = properties["city"].get<string>();
        string street = properties["street"].get<string>();
        string status = properties["dot"].get<string>();

        // TODO: make getStatusColor function
        Color color;
        if (status == "broken") {
            color = red;
        } else if (status == "working") {
            color = green;
        } else if (status == "inactive") {
            color = gray;
        }

        // TODO: make printResults function
        count++;

        if (status == "broken") {
            cout << "BROKEN: (" << hPoint << ", " << vPoint << ") " << street << ", " << city << ", " << state << endl;
        }

        // cout << endl;
        // cout << count << ". " << street << ", " << city << ", " << state << endl;
        // cout << "     " << "coords: (" << lat << ", " << lng << ")" << endl;
        // cout << "     " << "matrix: (" << hPoint << ", " << vPoint << ") " << status << "!" << endl;

        // light pixel for store with appropriate color
        canvas.SetPixel(hPoint, vPoint, color);

        // M
        canvas.SetPixel(50, 1, yellow);
        canvas.SetPixel(50, 2, yellow);
        canvas.SetPixel(50, 3, yellow);
        canvas.SetPixel(50, 4, yellow);
        canvas.SetPixel(50, 5, yellow);

        canvas.SetPixel(51, 2, yellow);
        canvas.SetPixel(52, 3, yellow);
        canvas.SetPixel(52, 4, yellow);
        canvas.SetPixel(53, 2, yellow);

        canvas.SetPixel(54, 1, yellow);
        canvas.SetPixel(54, 2, yellow);
        canvas.SetPixel(54, 3, yellow);
        canvas.SetPixel(54, 4, yellow);
        canvas.SetPixel(54, 5, yellow);

        // S
        canvas.SetPixel(56, 1, white);
        canvas.SetPixel(56, 2, white);
        canvas.SetPixel(56, 3, white);
        canvas.SetPixel(56, 5, white);

        canvas.SetPixel(57, 1, white);
        canvas.SetPixel(57, 3, white);
        canvas.SetPixel(57, 5, white);

        canvas.SetPixel(58, 1, white);
        canvas.SetPixel(58, 3, white);
        canvas.SetPixel(58, 4, white);
        canvas.SetPixel(58, 5, white);

        // P
        canvas.SetPixel(60, 1, white);
        canvas.SetPixel(60, 2, white);
        canvas.SetPixel(60, 3, white);
        canvas.SetPixel(60, 4, white);
        canvas.SetPixel(60, 5, white);

        canvas.SetPixel(61, 1, white);
        canvas.SetPixel(61, 3, white);

        canvas.SetPixel(62, 1, white);
        canvas.SetPixel(62, 2, white);
        canvas.SetPixel(62, 3, white);
    }

    canvas.Send();  // send the framebuffer
}
