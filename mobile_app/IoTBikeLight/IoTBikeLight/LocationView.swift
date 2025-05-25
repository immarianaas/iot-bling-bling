import SwiftUI
import MapKit

struct LocationView: View {
    @State private var bikeLocation = CLLocationCoordinate2D(latitude: 55.6761, longitude: 12.5683)
    @State private var position: MapCameraPosition
    @State private var locationSource: String = "ukendt"

    init() {
        _position = State(initialValue:
            MapCameraPosition.region(
                MKCoordinateRegion(
                    center: CLLocationCoordinate2D(latitude: 55.6761, longitude: 12.5683),
                    span: MKCoordinateSpan(latitudeDelta: 0.01, longitudeDelta: 0.01)
                )
            )
        )
    }

    var body: some View {
        VStack {
            Text("Bike Location")
                .font(.title)
                .padding()

            Map(position: $position) {
                Marker("Bike", coordinate: bikeLocation)
            }
            .frame(height: 670)

            Spacer()
        }
    }

    func fetchLocation() {
        guard let url = URL(string: "http://10.209.135.228:5050/location") else {
            print("Forkert URL")
            return
        }

        URLSession.shared.dataTask(with: url) { data, _, error in
            if let data = data {
                let responseText = String(data: data, encoding: .utf8) ?? "ukendt"
                print("Svar fra Flask: \(responseText)")

                if let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
                   let lat = json["lat"] as? Double,
                   let lng = json["lng"] as? Double {
                    let source = json["source"] as? String ?? "ukendt"

                    DispatchQueue.main.async {
                        bikeLocation = CLLocationCoordinate2D(latitude: lat, longitude: lng)
                        position = .region(
                            MKCoordinateRegion(
                                center: bikeLocation,
                                span: MKCoordinateSpan(latitudeDelta: 0.01, longitudeDelta: 0.01)
                            )
                        )
                        locationSource = source
                    }
                } else {
                    print("Kunne ikke parse koordinater fra JSON")
                }
            } else {
                print("Fejl ved hentning af lokation:", error ?? "ukendt fejl")
            }
        }.resume()
    }
}
