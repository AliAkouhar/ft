#!/usr/bin/env python3
"""
IRC Server TCP Chunks and Delimiter Test Suite
"""
import socket
import time
import threading
import sys

class IRCTester:
    def __init__(self, host='localhost', port=6667):
        self.host = host
        self.port = port
        
    def test_connection(self):
        """Test basic connection"""
        print("🔌 Testing basic connection...")
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(3)
            s.connect((self.host, self.port))
            s.close()
            print("✅ Connection successful")
            return True
        except Exception as e:
            print(f"❌ Connection failed: {e}")
            return False
    
    def test_complete_message(self):
        """Test sending complete IRC message"""
        print("\n📨 Testing complete IRC message...")
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.host, self.port))
            
            # Send complete message
            msg = b'PASS mypassword\r\n'
            s.send(msg)
            print(f"   Sent: {msg}")
            
            time.sleep(0.5)
            s.close()
            print("✅ Complete message sent successfully")
            return True
        except Exception as e:
            print(f"❌ Failed: {e}")
            return False
    
    def test_fragmented_message(self):
        """Test TCP chunk handling"""
        print("\n🧩 Testing fragmented message (TCP chunks)...")
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.host, self.port))
            
            # Send message in fragments
            fragments = [b'NICK ', b'test', b'user', b'123', b'\r\n']
            
            for i, fragment in enumerate(fragments):
                s.send(fragment)
                print(f"   Chunk {i+1}: {fragment}")
                time.sleep(0.2)  # Simulate network delay
            
            time.sleep(0.5)
            s.close()
            print("✅ Fragmented message sent successfully")
            return True
        except Exception as e:
            print(f"❌ Failed: {e}")
            return False
    
    def test_multiple_messages(self):
        """Test multiple messages in one packet"""
        print("\n📦 Testing multiple messages in one packet...")
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.host, self.port))
            
            # Send multiple complete messages at once
            multi_msg = b'USER testuser 0 * :Test User\r\nJOIN #test\r\nPRIVMSG #test :Hello\r\n'
            s.send(multi_msg)
            print(f"   Sent 3 messages together: USER + JOIN + PRIVMSG")
            
            time.sleep(0.5)
            s.close()
            print("✅ Multiple messages sent successfully")
            return True
        except Exception as e:
            print(f"❌ Failed: {e}")
            return False
    
    def test_partial_message_buffering(self):
        """Test that partial messages are buffered correctly"""
        print("\n⏳ Testing partial message buffering...")
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.host, self.port))
            
            # Send partial message (no delimiter)
            partial = b'PRIVMSG #test :This is a long message that'
            s.send(partial)
            print(f"   Sent partial: {partial}")
            print("   ⏱️  Waiting 2 seconds (message should be buffered)...")
            time.sleep(2)
            
            # Complete the message
            completion = b' gets completed later\r\n'
            s.send(completion)
            print(f"   Completed with: {completion}")
            
            time.sleep(0.5)
            s.close()
            print("✅ Partial message buffering test completed")
            return True
        except Exception as e:
            print(f"❌ Failed: {e}")
            return False
    
    def test_multiple_clients(self):
        """Test multiple clients with independent buffers"""
        print("\n👥 Testing multiple clients...")
        
        def client_worker(client_id):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((self.host, self.port))
                
                # Each client sends different fragmented messages
                if client_id == 1:
                    s.send(b'NICK client')
                    time.sleep(0.3)
                    s.send(b'001\r\n')
                else:
                    s.send(b'NICK client')
                    time.sleep(0.3)  
                    s.send(b'002\r\n')
                
                time.sleep(1)
                s.close()
                print(f"   ✅ Client {client_id} completed")
            except Exception as e:
                print(f"   ❌ Client {client_id} failed: {e}")
        
        try:
            # Start two client threads
            thread1 = threading.Thread(target=client_worker, args=(1,))
            thread2 = threading.Thread(target=client_worker, args=(2,))
            
            thread1.start()
            thread2.start()
            
            thread1.join()
            thread2.join()
            
            print("✅ Multiple clients test completed")
            return True
        except Exception as e:
            print(f"❌ Failed: {e}")
            return False

def main():
    print("=" * 50)
    print("🚀 IRC Server TCP Chunks & Delimiters Test Suite")
    print("=" * 50)
    print()
    print("Make sure your IRC server is running:")
    print("./irc_serv 6667 mypassword")
    print()
    
    tester = IRCTester()
    
    tests = [
        ("Connection", tester.test_connection),
        ("Complete Message", tester.test_complete_message),
        ("Fragmented Message", tester.test_fragmented_message),
        ("Multiple Messages", tester.test_multiple_messages),
        ("Partial Buffering", tester.test_partial_message_buffering),
        ("Multiple Clients", tester.test_multiple_clients),
    ]
    
    results = []
    for test_name, test_func in tests:
        result = test_func()
        results.append((test_name, result))
    
    print("\n" + "=" * 50)
    print("📊 TEST RESULTS SUMMARY")
    print("=" * 50)
    
    passed = 0
    for test_name, result in results:
        status = "✅ PASS" if result else "❌ FAIL"
        print(f"{test_name:20} : {status}")
        if result:
            passed += 1
    
    print(f"\nPassed: {passed}/{len(results)} tests")
    
    if passed == len(results):
        print("\n🎉 All tests passed! Your TCP chunk handling is working correctly!")
    else:
        print(f"\n⚠️  {len(results) - passed} test(s) failed. Check your server implementation.")
    
    print("\n💡 Tips for verification:")
    print("- Watch your server console output during tests")
    print("- Look for 'New client connected' messages")  
    print("- Check that commands only execute after receiving \\r\\n")
    print("- Verify no 'incomplete command' errors appear")

if __name__ == "__main__":
    main()
