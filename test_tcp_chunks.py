#!/usr/bin/env python3
import socket
import time
import sys

def test_tcp_chunks():
    """Test TCP chunk handling and IRC delimiter processing"""
    
    print("=== IRC Server TCP Chunk Test ===")
    
    try:
        # Connect to server
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', 6667))
        print("✓ Connected to IRC server")
        
        # Test 1: Normal complete message
        print("\n1. Testing complete message:")
        s.send(b'PASS mypassword\r\n')
        time.sleep(0.1)
        print("   Sent: PASS mypassword\\r\\n")
        
        # Test 2: Fragmented message (TCP chunks)
        print("\n2. Testing fragmented message:")
        s.send(b'NICK test')  # Partial message
        print("   Sent chunk 1: 'NICK test' (no delimiter)")
        time.sleep(0.1)
        
        s.send(b'user123')    # Continue message
        print("   Sent chunk 2: 'user123' (still no delimiter)")
        time.sleep(0.1)
        
        s.send(b'\r\n')       # Complete with delimiter
        print("   Sent chunk 3: '\\r\\n' (delimiter)")
        time.sleep(0.1)
        
        # Test 3: Multiple messages in one packet
        print("\n3. Testing multiple messages in one packet:")
        multi_msg = b'USER testuser123 0 * :Test User\r\nJOIN #test\r\n'
        s.send(multi_msg)
        print("   Sent: USER command + JOIN command together")
        time.sleep(0.1)
        
        # Test 4: Partial message that stays in buffer
        print("\n4. Testing partial message (should stay in buffer):")
        s.send(b'PRIVMSG #test :Hello')  # No delimiter
        print("   Sent: 'PRIVMSG #test :Hello' (no delimiter - should stay in buffer)")
        time.sleep(0.5)
        
        print("\n5. Completing the partial message:")
        s.send(b' World!\r\n')
        print("   Sent: ' World!\\r\\n' (completing the message)")
        time.sleep(0.1)
        
        # Clean up
        s.close()
        print("\n✓ Connection closed")
        print("\n=== Test completed ===")
        print("Check your IRC server output to see how it handled the chunks!")
        
    except ConnectionRefusedError:
        print("✗ Could not connect to IRC server on localhost:6667")
        print("Make sure your IRC server is running with: ./irc_serv 6667 mypassword")
        sys.exit(1)
    except Exception as e:
        print(f"✗ Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    test_tcp_chunks()
