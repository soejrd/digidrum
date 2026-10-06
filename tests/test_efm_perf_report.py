import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import efm_perf_report as perf


class EfmPerfReportTest(unittest.TestCase):
    def test_pack7_round_trip(self):
        data = bytes(range(256))
        self.assertEqual(perf.unpack7(perf.pack7(data)), data)

    def test_peek_reply_and_table(self):
        address = 0x47be8000
        table = bytearray(perf.TABLE_BYTES)
        struct.pack_into('>IIIII', table, 2 * perf.SLOT_BYTES,
                         100, 100, 25000, 200, 320)
        body = struct.pack('>HBBI', 1, 0x83, 0, address) + table
        reply = perf.HEADER + perf.pack7(body) + b'\xf7'
        payload = perf.decode_reply(reply, 1, 3)
        self.assertEqual(struct.unpack_from('>I', payload)[0], address)
        self.assertEqual(perf.parse_table(payload[4:])[0][2],
                         (100, 100, 25000, 200, 320))


if __name__ == '__main__':
    unittest.main()
