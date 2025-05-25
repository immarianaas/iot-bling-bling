import SwiftUI

struct ContentView: View {
    var body: some View {
        TabView {
            LocationView()
                .tabItem {
                    Label("Location", systemImage: "location")
                }

            StatusView()
                .tabItem {
                    Label("Status", systemImage: "bolt.batteryblock")
                }
        }
    }
}

#Preview {
    ContentView()
}
