import SwiftUI

enum BikeMode: String, CaseIterable {
    case active = "Active"
    case park = "Park"
    case storage = "Storage"
}

struct StatusView: View {
    @State private var selectedMode: BikeMode = .active
    @State private var batteryLevel: Int = -1

    var body: some View {
        VStack(spacing: 20) {
            Text("Bike Status")
                .font(.largeTitle)
                .padding(.top)
            VStack(alignment: .leading, spacing: 10) {

                HStack {
                    Button("Request Buzzer") {
                        sendDownlinkToDevice(hexPayload: "01")
                    }
                    .frame(maxWidth: .infinity)
                    .padding()

                    Button("Request location") {
                        sendDownlinkToDevice(hexPayload: "02")
                    }
                    .frame(maxWidth: .infinity)
                    .padding()
                }
            }
            .padding(.horizontal)

            VStack {
                Text("Battery")
                    .font(.title2)
                    .foregroundColor(.gray)
                Text(batteryLevel >= 0 ? "\(batteryLevel)%" : "??%")
                    .font(.system(size: 48, weight: .bold))
                    .foregroundColor(batteryLevel >= 0 && batteryLevel < 20 ? .red : .primary)
            }
            .frame(maxWidth: .infinity)
            .padding(.vertical, 40)

            Spacer()
        }
        .onAppear {
            fetchBatteryLevel()
        }
    }

    func fetchBatteryLevel() {
        guard let url = URL(string: "http://10.209.135.228:5050/battery") else {
            print("Invalid battery URL")
            return
        }

        URLSession.shared.dataTask(with: url) { data, response, error in
            if let data = data,
               let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
               let battery = json["battery"] as? Int {
                DispatchQueue.main.async {
                    batteryLevel = battery
                }
            } else {
                print("No battery data available")
                DispatchQueue.main.async {
                    batteryLevel = -1
                }
            }
        }.resume()
    }
}

func sendDownlinkToDevice(hexPayload: String) {
    guard let url = URL(string: "http://10.209.135.228:5050/send-to-lorawan") else {
        print("Invalid URL")
        return
    }

    var request = URLRequest(url: url)
    request.httpMethod = "POST"
    request.addValue("application/json", forHTTPHeaderField: "Content-Type")

    let json: [String: Any] = ["payload": hexPayload]
    request.httpBody = try? JSONSerialization.data(withJSONObject: json)

    URLSession.shared.dataTask(with: request) { data, response, error in
        if let error = error {
            print("Error:", error)
        } else if let response = response as? HTTPURLResponse {
            print("Response code:", response.statusCode)
            if let data = data, let text = String(data: data, encoding: .utf8) {
                print("Response from Flask:", text)
            }
        }
    }.resume()
}

struct StatusView_Previews: PreviewProvider {
    static var previews: some View {
        StatusView()
    }
}
