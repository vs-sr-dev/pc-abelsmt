"""ABEL's own files, read: the text of the .MES files and of the maps (.ABM), the maps' chunks,
the TIFF pictures.

    python tools/abelfiles.py text build/disc/ABEL               # every text record, checked; the census
    python tools/abelfiles.py text build/disc/ABEL --tsv OUT.tsv # the records: id, speaker, text
    python tools/abelfiles.py abm build/disc/ABEL/MAP/0/A01.ABM  # a map's chunks
    python tools/abelfiles.py tif build/disc/ABEL/OP/OP01.TIF OUT.png

**Text** (docs/02): a .MES file is records, `u32 speaker` then Shift-JIS text ended by a NUL;
its .MID is the index, `u32 id, u32 n, u32 offset` twelve bytes each: n records from the
offset. A map (.ABM) carries the same pair as its chunks .BIN (the records) and .IDX (the
index). An index entry with no records (n = 0) owns the bytes to the next entry: dead (one NUL
in M03.ABM, an old line with no speaker in Z04.ABM). Every byte of every text is read, once.
A record's id for a translation table: `FILE:ID.K` (the index entry's id, the k-th record).

**Maps**: 'ABEL TOWNS V1.00' and 16 bytes more, then chunks `.XYZ, u32 size, data` to the
file's end: .RGB .PAT .CBM .DAT .DIR .EVL .EVT .IDX .BIN (or .CBM .DAT alone).

**TIFF**: uncompressed, a palette; 8 bits, or 4 with FillOrder 2 (the low nibble the first
pixel), which PIL does not decode.

The output holds the game's text and pictures: write it under build/ (BYOA).
"""
import argparse
import os
import re
import struct
import sys

CODE = re.compile(rb'\\(?:[A-Z][0-9]{2}|[A-Z@])')


def index(b, o, end):
    return [struct.unpack_from('<III', b, k) for k in range(o, end, 12)]


def records(b, base, end, idx):
    """[(offset, speaker, text, id, k)] as the index reads them, or ValueError."""
    out, o = [], base
    ents = sorted(idx, key=lambda x: x[2])
    for j, (i, n, off) in enumerate(ents):
        if base + off != o:
            raise ValueError('index %d at %#x, the last record ended at %#x' % (i, off, o - base))
        if n == 0:
            o = base + ents[j + 1][2] if j + 1 < len(ents) else end
        for k in range(n):
            tag, = struct.unpack_from('<I', b, o)
            e = b.index(b'\0', o + 4)
            out.append((o - base, tag, b[o + 4:e], i, k))
            o = e + 1
    if o != end:
        raise ValueError('the records end at %#x, their chunk at %#x' % (o - base, end - base))
    return out


def chunks(b):
    if b[:16] != b'ABEL TOWNS V1.00':
        raise ValueError('not an ABEL map (%r)' % b[:16])
    out, o = {}, 0x20
    while o < len(b):
        size, = struct.unpack_from('<I', b, o + 4)
        out[b[o:o + 4].decode('latin-1')] = (o + 8, size)
        o += 8 + size
    if o != len(b):
        raise ValueError('the chunks end at %#x, the file at %#x' % (o, len(b)))
    return out


def text_of(path):
    """A .MES (with its .MID) or an .ABM: (index, records), or None for a map with no text."""
    b = open(path, 'rb').read()
    if path.upper().endswith('.MES'):
        m = open(path[:-4] + '.MID', 'rb').read()
        idx = index(m, 0, len(m))
        return idx, records(b, 0, len(b), idx)
    ch = chunks(b)
    if '.BIN' not in ch:
        return None
    (bo, bs), (io, isz) = ch['.BIN'], ch['.IDX']
    idx = index(b, io, io + isz)
    return idx, records(b, bo, bo + bs, idx)


def chars(t):
    t = CODE.sub(b'', t)
    n = k = 0
    while k < len(t):
        k += 2 if 0x81 <= t[k] <= 0x9F or 0xE0 <= t[k] <= 0xFC else 1
        n += 1
    return n


def cmd_text(a):
    tot, codes, speakers, rows = {}, {}, {}, []
    for d, _, fs in os.walk(a.dir):
        for f in sorted(fs):
            if not f.upper().endswith(('.MES', '.ABM')):
                continue
            p = os.path.join(d, f)
            got = text_of(p)
            if got is None:
                continue
            idx, recs = got
            kind = f.upper()[-3:]
            t = tot.setdefault(kind, [0, 0, 0, 0])
            t[0] += 1
            t[1] += len(idx)
            t[2] += len(recs)
            rel = os.path.relpath(p, a.dir).replace(os.sep, '/')
            for off, tag, txt, i, k in recs:
                n = chars(txt)
                t[3] += n
                speakers[tag] = speakers.get(tag, 0) + n
                for c in CODE.findall(txt):
                    codes[c] = codes.get(c, 0) + 1
                rows.append('%s:%d.%d\t%d\t%s' % (rel, i, k, tag, txt.decode('cp932', 'replace')))
    for kind, (nf, ns, nr, nc) in sorted(tot.items()):
        print('%s: %d files, %d index entries, %d records, %d characters (control codes aside)'
              % (kind, nf, ns, nr, nc))
    print('control codes:', ' '.join('%s %d' % (k.decode(), v) for k, v in sorted(codes.items())))
    print('speakers: %d; the five with most text: %s' % (len(speakers), ', '.join(
        '%d (%d)' % kv for kv in sorted(speakers.items(), key=lambda kv: -kv[1])[:5])))
    if a.tsv:
        with open(a.tsv, 'w', encoding='utf-8') as f:
            f.write('id\tspeaker\ttext\n' + '\n'.join(rows) + '\n')
        print('%d records in %s' % (len(rows), a.tsv))


def cmd_abm(a):
    b = open(a.file, 'rb').read()
    ch = chunks(b)
    print('%s: %d bytes, %s' % (a.file, len(b), b[:16].decode('latin-1')))
    for tag, (o, s) in ch.items():
        print('  %s at %#07x, %d bytes' % (tag, o, s))


def tif(path):
    from PIL import Image
    import numpy as np
    im = Image.open(path)
    t = im.tag_v2
    w, h, bps = t[256], t[257], t[258][0]
    if bps == 8:
        return im
    if bps != 4 or t.get(259, 1) != 1:
        raise ValueError('%s: %d bits, compression %d: not read here' % (path, bps, t.get(259, 1)))
    raw = open(path, 'rb').read()[t[273][0]:t[273][0] + t[279][0]]
    stride = (w + 1) // 2
    a = np.frombuffer(raw[:stride * h].ljust(stride * h, b'\0'), dtype=np.uint8).reshape(h, stride)
    lo, hi = a & 15, a >> 4
    first, second = (lo, hi) if t.get(266, 1) == 2 else (hi, lo)
    px = np.stack([first, second], axis=2).reshape(h, stride * 2)[:, :w]
    out = Image.fromarray(np.ascontiguousarray(px), 'P')
    cm = t[320]
    n = len(cm) // 3
    out.putpalette([cm[k * n + i] >> 8 for i in range(n) for k in range(3)])
    return out


def cmd_tif(a):
    tif(a.file).convert('RGB').save(a.out)
    print(a.out)


def main(argv=None):
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('text')
    p.add_argument('dir', help="the disc's ABEL directory")
    p.add_argument('--tsv', metavar='OUT')
    p = sub.add_parser('abm')
    p.add_argument('file')
    p = sub.add_parser('tif')
    p.add_argument('file')
    p.add_argument('out')
    a = ap.parse_args(argv)
    {'text': cmd_text, 'abm': cmd_abm, 'tif': cmd_tif}[a.cmd](a)


if __name__ == '__main__':
    main()
