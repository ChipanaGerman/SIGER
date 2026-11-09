#!/usr/bin/env python3
"""
generar_rutas_osrm.py
Calcula y cachea las rutas vehiculares reales (polilíneas calle a calle) entre
los 20 nodos logísticos metropolitanos de Arequipa utilizando la API de OSRM
(Open Source Routing Machine) y proyectándolas a las coordenadas de píxeles
de la escena gráfica de SIGER (Web Mercator 1:1, 1536x1536 px).
"""

import os
import json
import math
import time
import urllib.request
from concurrent.futures import ThreadPoolExecutor, as_completed

NODES = {
    "Plaza de Armas": (-71.5369, -16.3988),
    "Calle Mercaderes 120": (-71.5348, -16.3980),
    "Calle San Francisco 204": (-71.5350, -16.3958),
    "Mirador de Yanahuara": (-71.5422, -16.3882),
    "Av. Ejercito 450": (-71.5445, -16.3912),
    "Av. Cayma 310": (-71.5475, -16.3835),
    "Urb. Umacollo B-12": (-71.5470, -16.4025),
    "UNSA Ingenierias (Av. Independencia)": (-71.5280, -16.4052),
    "Av. Venezuela 800 (UNSA)": (-71.5215, -16.4095),
    "Av. Lambramani 105": (-71.5218, -16.4162),
    "Mall Aventura Porongoche": (-71.5125, -16.4258),
    "Av. Dolores 890": (-71.5248, -16.4215),
    "Plaza Mayta Capac (Miraflores)": (-71.5180, -16.3925),
    "Parque Selva Alegre": (-71.5315, -16.3875),
    "Mirador de Sachaca": (-71.5645, -16.4252),
    "Terminal Terrestre Arequipa": (-71.5492, -16.4275),
    "Cerro Colorado (Plaza Las Americas)": (-71.5598, -16.3655),
    "Parque Industrial": (-71.5845, -16.3548),
    "Plaza de Paucarpata": (-71.4985, -16.4328),
    "Molino de Sabandia": (-71.4880, -16.4520)
}

ZOOM = 14
X_MIN, Y_MIN = 4933, 8946
TILE_SIZE = 256
WIDTH = 6 * TILE_SIZE   # 1536
HEIGHT = 6 * TILE_SIZE  # 1536

def geo_to_scene(lat, lon):
    lat_rad = math.radians(lat)
    n = 2.0 ** ZOOM
    x_tile = (lon + 180.0) / 360.0 * n
    y_tile = (1.0 - math.asinh(math.tan(lat_rad)) / math.pi) / 2.0 * n
    px = (x_tile - X_MIN) * TILE_SIZE
    py = (y_tile - Y_MIN) * TILE_SIZE
    return round(px - WIDTH / 2.0, 1), round(py - HEIGHT / 2.0, 1)

def query_osrm(orig_name, dest_name):
    lon1, lat1 = NODES[orig_name]
    lon2, lat2 = NODES[dest_name]
    url = f"http://router.project-osrm.org/route/v1/driving/{lon1},{lat1};{lon2},{lat2}?overview=full&geometries=geojson"
    req = urllib.request.Request(url, headers={'User-Agent': 'SIGER-UNSA/1.0 (siger@unsa.edu.pe)'})
    
    for intento in range(3):
        try:
            with urllib.request.urlopen(req, timeout=10) as resp:
                data = json.loads(resp.read().decode('utf-8'))
                if data.get('code') == 'Ok' and data.get('routes'):
                    route = data['routes'][0]
                    coords = route['geometry']['coordinates'] # [ [lon, lat], ... ]
                    
                    # Convertir a coordenadas de escena
                    pts_escena = []
                    # Asegurar inicio exacto en el pin de origen
                    p_orig = geo_to_scene(lat1, lon1)
                    pts_escena.append(p_orig)
                    
                    for pt in coords:
                        sp = geo_to_scene(pt[1], pt[0])
                        # Evitar puntos duplicados consecutivos
                        if not pts_escena or (pts_escena[-1] != sp):
                            pts_escena.append(sp)
                            
                    # Asegurar fin exacto en el pin de destino
                    p_dest = geo_to_scene(lat2, lon2)
                    if not pts_escena or (pts_escena[-1] != p_dest):
                        pts_escena.append(p_dest)
                        
                    return {
                        "distancia_m": round(route.get('distance', 0), 1),
                        "duracion_s": round(route.get('duration', 0), 1),
                        "puntos": pts_escena
                    }
        except Exception as e:
            time.sleep(0.5 * (intento + 1))
            
    # Fallback si la API falló tras 3 intentos: línea recta conectada
    p1 = geo_to_scene(lat1, lon1)
    p2 = geo_to_scene(lat2, lon2)
    return {
        "distancia_m": 1000.0,
        "duracion_s": 120.0,
        "puntos": [p1, p2]
    }

def main():
    output_path = os.path.join(os.path.dirname(__file__), "..", "resources", "rutas_arequipa.json")
    output_path = os.path.abspath(output_path)
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    rutas = {}
    if os.path.exists(output_path):
        try:
            with open(output_path, "r", encoding="utf-8") as f:
                rutas = json.load(f)
            print(f"Rutas ya cacheadas previamente: {len(rutas)}")
        except Exception:
            rutas = {}

    pairs_to_query = []
    node_names = list(NODES.keys())
    for o in node_names:
        for d in node_names:
            if o != d:
                key = f"{o}|{d}"
                if key not in rutas:
                    pairs_to_query.append((o, d))

    print(f"Total pares requeridos: {len(node_names) * (len(node_names) - 1)}")
    print(f"Pares pendientes de consultar: {len(pairs_to_query)}")

    if not pairs_to_query:
        print("Todas las rutas ya están generadas.")
        return

    # Consulta con pool concurrente de 3 trabajadores para no saturar OSRM
    completados = 0
    t0 = time.time()
    with ThreadPoolExecutor(max_workers=3) as executor:
        future_to_pair = {
            executor.submit(query_osrm, o, d): (o, d)
            for o, d in pairs_to_query
        }
        
        for future in as_completed(future_to_pair):
            o, d = future_to_pair[future]
            try:
                res = future.result()
                key = f"{o}|{d}"
                rutas[key] = res
                completados += 1
                if completados % 25 == 0 or completados == len(pairs_to_query):
                    elapsed = time.time() - t0
                    print(f"Progreso: {completados}/{len(pairs_to_query)} rutas ({completados/len(pairs_to_query)*100:.1f}%) - {elapsed:.1f}s transcurridos")
                    # Guardar progreso parcial
                    with open(output_path, "w", encoding="utf-8") as f:
                        json.dump(rutas, f, ensure_ascii=False)
            except Exception as exc:
                print(f"Error procesando {o} -> {d}: {exc}")

    # Guardar archivo final
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(rutas, f, ensure_ascii=False, indent=2)

    tamano_kb = os.path.getsize(output_path) / 1024
    print(f"¡Completado con éxito! {len(rutas)} rutas guardadas en {output_path} ({tamano_kb:.1f} KB).")

if __name__ == "__main__":
    main()
