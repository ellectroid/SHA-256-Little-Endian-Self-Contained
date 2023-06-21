# SHA-256-Little-Endian-Self-Contained

Basic self-contained implementation of SHA-256.

Requirements: -fno-strict-aliasing

Properties: 
- Implementation for little-endian system
- Only byte data access, no memory alignment required
- All memory, including for constants, is allocated on stack on function call (easily modifiable)
- 32-bit input data byte length (easily modifiable)
