#!/usr/bin/env python3
"""
Simple test to demonstrate buffer handling and client information display
"""
import socket
import time

def test_buffer_display():
    print("Testing IRC Server Buffer Display...")
    print("====================================")
    
    try:
        # Connect to server
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', 6667))
        print("Connected to server")
        
        # Test 1: Send partial message (should show in partial buffer)
        print("\n1. Sending partial message...")
        s.send(b'PASS mypass')
        time.sleep(1)
        
        # Test 2: Complete the message
        print("2. Completing the message...")
        s.send(b'word\r\n')
        time.sleep(1)
        
        # Test 3: Send fragmented message
        print("3. Sending fragmented NICK command...")
        s.send(b'NICK ')
        time.sleep(0.5)
        s.send(b'testuser')
        time.sleep(0.5)
        s.send(b'123\r\n')
        time.sleep(1)
        
        # Test 4: Multiple messages at once
        print("4. Sending multiple messages at once...")
        s.send(b'USER test 0 * :Test\r\nJOIN #test\r\n')
        time.sleep(1)
        
        # Test 5: Partial message that will stay in buffer
        print("5. Sending partial message (will stay in buffer)...")
        s.send(b'PRIVMSG #test :Hello')
        time.sleep(2)
        
        s.close()
        print("Test completed!")
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    test_buffer_display()
