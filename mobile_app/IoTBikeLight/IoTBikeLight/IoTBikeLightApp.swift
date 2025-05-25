//
//  IoTBikeLightApp.swift
//  IoTBikeLight
//
//  Created by Vandad Azar on 27/03/2025.
//

import SwiftUI

@main
struct IoTBikeLightApp: App {
    let socket = WebSocketManager()
    init(){
        socket.connect()
    }
    
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}
