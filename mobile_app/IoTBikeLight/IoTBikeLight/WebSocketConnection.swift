import Foundation

class WebSocketManager: NSObject, URLSessionWebSocketDelegate {
    var webSocket: URLSessionWebSocketTask?

    func connect() {
        let url = URL(string: "wss://iotnet.teracom.dk/app?token=vnoWTwAAABFpb3RuZXQudGVyYWNvbS5ka3X66ebIRRamZlg1c9ycJi4=")!
        let session = URLSession(configuration: .default, delegate: self, delegateQueue: OperationQueue())
        webSocket = session.webSocketTask(with: url)
        webSocket?.resume()
        receive()
    }

    func receive() {
        webSocket?.receive { [weak self] result in
            switch result {
            case .failure(let error):
                print("WebSocket fejl:", error)
            case .success(let message):
                switch message {
                case .string(let text):
                    print("Modtaget tekst:", text)
                default:
                    break
                }
                self?.receive()
            }
        }
    }

    func disconnect() {
        webSocket?.cancel(with: .goingAway, reason: nil)
    }
}
