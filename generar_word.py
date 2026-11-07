# -*- coding: utf-8 -*-
"""
Script para generar el Informe Académico de Avance en formato Microsoft Word (.docx)
para el proyecto SIGER - Tecnología de Objetos (UNSA).
"""

import os
import docx
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

def set_cell_background(cell, fill_hex):
    """Establece el color de fondo de una celda."""
    shading_elm = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
    cell._tc.get_or_add_tcPr().append(shading_elm)

def set_cell_margins(cell, top=120, bottom=120, left=150, right=150):
    """Establece márgenes internos de una celda en dxa."""
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = parse_xml(f'''
        <w:tcMar {nsdecls("w")}>
            <w:top w:w="{top}" w:type="dxa"/>
            <w:bottom w:w="{bottom}" w:type="dxa"/>
            <w:left w:w="{left}" w:type="dxa"/>
            <w:right w:w="{right}" w:type="dxa"/>
        </w:tcMar>
    ''')
    tcPr.append(tcMar)

def add_callout(doc, title, text, border_color="1A73E8", bg_color="F1F3F4"):
    """Agrega un cuadro de nota / callout estilizado."""
    table = doc.add_table(rows=1, cols=1)
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.autofit = False
    
    cell = table.cell(0, 0)
    cell.width = Inches(6.5)
    set_cell_background(cell, bg_color)
    set_cell_margins(cell, top=140, bottom=140, left=200, right=200)
    
    # Borde izquierdo grueso
    tcPr = cell._tc.get_or_add_tcPr()
    borders = parse_xml(f'''
        <w:tcBorders {nsdecls("w")}>
            <w:top w:val="none"/>
            <w:left w:val="single" w:sz="24" w:space="0" w:color="{border_color}"/>
            <w:bottom w:val="none"/>
            <w:right w:val="none"/>
        </w:tcBorders>
    ''')
    tcPr.append(borders)
    
    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(3)
    p.paragraph_format.line_spacing = 1.15
    run_t = p.add_run(f"📌 {title}\n")
    run_t.bold = True
    run_t.font.name = "Calibri"
    run_t.font.size = Pt(10.5)
    run_t.font.color.rgb = RGBColor(0x1A, 0x73, 0xE8)
    
    run_body = p.add_run(text)
    run_body.font.name = "Calibri"
    run_body.font.size = Pt(9.5)
    run_body.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
    
    # Espacio tras la tabla
    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(6)

def add_code_block(doc, code_str, language="C++"):
    """Agrega un bloque de código estilizado."""
    table = doc.add_table(rows=1, cols=1)
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.autofit = False
    
    cell = table.cell(0, 0)
    cell.width = Inches(6.5)
    set_cell_background(cell, "272822") # Fondo oscuro tipo IDE
    set_cell_margins(cell, top=120, bottom=120, left=180, right=180)
    
    tcPr = cell._tc.get_or_add_tcPr()
    borders = parse_xml(f'''
        <w:tcBorders {nsdecls("w")}>
            <w:top w:val="single" w:sz="4" w:space="0" w:color="3E3D32"/>
            <w:left w:val="single" w:sz="12" w:space="0" w:color="66D9EF"/>
            <w:bottom w:val="single" w:sz="4" w:space="0" w:color="3E3D32"/>
            <w:right w:val="single" w:sz="4" w:space="0" w:color="3E3D32"/>
        </w:tcBorders>
    ''')
    tcPr.append(borders)
    
    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(2)
    p.paragraph_format.line_spacing = 1.05
    
    run_hdr = p.add_run(f"// [{language}] Código de Implementación en SIGER\n")
    run_hdr.font.name = "Consolas"
    run_hdr.font.size = Pt(8.5)
    run_hdr.font.color.rgb = RGBColor(0x75, 0x71, 0x5E)
    run_hdr.italic = True
    
    run_code = p.add_run(code_str)
    run_code.font.name = "Consolas"
    run_code.font.size = Pt(8.5)
    run_code.font.color.rgb = RGBColor(0xF8, 0xF8, 0xF2)
    
    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(6)

def main():
    doc = Document()
    
    # ── Configuración de Márgenes de Página ──
    sections = doc.sections
    for section in sections:
        section.top_margin = Inches(1.0)
        section.bottom_margin = Inches(1.0)
        section.left_margin = Inches(1.0)
        section.right_margin = Inches(1.0)
        section.different_first_page_header_footer = True
        
        # Pie de página para páginas interiores
        footer = section.footer
        p_foot = footer.paragraphs[0]
        p_foot.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        p_foot.paragraph_format.space_before = Pt(0)
        p_foot.paragraph_format.space_after = Pt(0)
        r_foot = p_foot.add_run("SIGER — Informe de Avance | Tecnología de Objetos UNSA")
        r_foot.font.name = "Calibri"
        r_foot.font.size = Pt(8.5)
        r_foot.font.color.rgb = RGBColor(0x88, 0x88, 0x88)

    # ─────────────────────────────────────────────────────────────────────────
    # 1. PORTADA ACADÉMICA FORMAL
    # ─────────────────────────────────────────────────────────────────────────
    p_univ = doc.add_paragraph()
    p_univ.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_univ.paragraph_format.space_before = Pt(10)
    p_univ.paragraph_format.space_after = Pt(2)
    r = p_univ.add_run("UNIVERSIDAD NACIONAL DE SAN AGUSTÍN DE AREQUIPA")
    r.bold = True
    r.font.name = "Calibri"
    r.font.size = Pt(16)
    r.font.color.rgb = RGBColor(0x0D, 0x47, 0xA1) # Azul institucional

    p_fac = doc.add_paragraph()
    p_fac.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fac.paragraph_format.space_before = Pt(0)
    p_fac.paragraph_format.space_after = Pt(2)
    r = p_fac.add_run("FACULTAD DE INGENIERÍA DE PRODUCCIÓN Y SERVICIOS\nESCUELA PROFESIONAL DE CIENCIA DE LA COMPUTACIÓN")
    r.bold = True
    r.font.name = "Calibri"
    r.font.size = Pt(12)
    r.font.color.rgb = RGBColor(0x33, 0x33, 0x33)

    p_line = doc.add_paragraph()
    p_line.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_line.paragraph_format.space_before = Pt(8)
    p_line.paragraph_format.space_after = Pt(18)
    r = p_line.add_run("━" * 50)
    r.font.color.rgb = RGBColor(0x0D, 0x47, 0xA1)

    p_curso = doc.add_paragraph()
    p_curso.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_curso.paragraph_format.space_before = Pt(0)
    p_curso.paragraph_format.space_after = Pt(6)
    r = p_curso.add_run("ASIGNATURA: TECNOLOGÍA DE OBJETOS")
    r.bold = True
    r.font.name = "Calibri"
    r.font.size = Pt(13)
    r.font.color.rgb = RGBColor(0x55, 0x55, 0x55)

    p_tit = doc.add_paragraph()
    p_tit.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_tit.paragraph_format.space_before = Pt(16)
    p_tit.paragraph_format.space_after = Pt(6)
    r = p_tit.add_run("PROYECTO FINAL:\nSISTEMA INTEGRADO DE GESTIÓN Y SIMULACIÓN DE ENTREGAS RÁPIDAS (SIGER)")
    r.bold = True
    r.font.name = "Calibri"
    r.font.size = Pt(18)
    r.font.color.rgb = RGBColor(0x15, 0x65, 0xC0)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_before = Pt(0)
    p_sub.paragraph_format.space_after = Pt(28)
    r = p_sub.add_run("INFORME ACADÉMICO DE AVANCE Y MEMORIA TÉCNICA DE IMPLEMENTACIÓN")
    r.italic = True
    r.font.name = "Calibri"
    r.font.size = Pt(11.5)
    r.font.color.rgb = RGBColor(0x66, 0x66, 0x66)

    # Tabla de Integrantes en Portada
    t_equipo = doc.add_table(rows=5, cols=3)
    t_equipo.alignment = WD_TABLE_ALIGNMENT.CENTER
    t_equipo.autofit = False
    
    headers = ["Estudiante / Integrante", "Correo Institucional", "Módulo de Especialidad Curricular"]
    col_widths = [Inches(2.3), Inches(2.0), Inches(2.2)]
    
    for c_idx, title in enumerate(headers):
        cell = t_equipo.cell(0, c_idx)
        cell.width = col_widths[c_idx]
        set_cell_background(cell, "0D47A1")
        set_cell_margins(cell, top=100, bottom=100, left=120, right=120)
        p = cell.paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.LEFT
        r = p.add_run(title)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(9.5)
        r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    datos_equipo = [
        ("Luis Fernando Baca Cruz", "lbacac@unsa.edu.pe", "POO Avanzada & Jerarquía de Dominio"),
        ("Gabriel Alexis Mejía Ramos", "gmejiar@unsa.edu.pe", "Programación Genérica & STL Memory"),
        ("Rodrigo Alonso Ponce López", "rponcel@unsa.edu.pe", "MultiThreading & Patrones GoF"),
        ("German Chipana Jamachi", "gchipanaj@unsa.edu.pe", "Arquitectura MVC & GUI en Qt 6")
    ]

    for r_idx, (nombre, correo, rol) in enumerate(datos_equipo, start=1):
        bg = "F5F7FA" if r_idx % 2 == 1 else "FFFFFF"
        for c_idx, val in enumerate([nombre, correo, rol]):
            cell = t_equipo.cell(r_idx, c_idx)
            cell.width = col_widths[c_idx]
            set_cell_background(cell, bg)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            r = p.add_run(val)
            r.font.name = "Calibri"
            r.font.size = Pt(9.0)
            if c_idx == 0:
                r.bold = True

    p_repo = doc.add_paragraph()
    p_repo.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_repo.paragraph_format.space_before = Pt(30)
    p_repo.paragraph_format.space_after = Pt(4)
    r = p_repo.add_run("Repositorio Oficial en GitHub:\n")
    r.font.name = "Calibri"
    r.font.size = Pt(10)
    r.bold = True
    r_link = p_repo.add_run("https://github.com/ChipanaGerman/SIGER")
    r_link.font.name = "Consolas"
    r_link.font.size = Pt(9.5)
    r_link.font.color.rgb = RGBColor(0x02, 0x88, 0xD1)

    p_fecha = doc.add_paragraph()
    p_fecha.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fecha.paragraph_format.space_before = Pt(14)
    p_fecha.paragraph_format.space_after = Pt(0)
    r = p_fecha.add_run("Arequipa — Perú\nNoviembre de 2026")
    r.font.name = "Calibri"
    r.font.size = Pt(10)
    r.font.color.rgb = RGBColor(0x55, 0x55, 0x55)

    doc.add_page_break()

    # ─────────────────────────────────────────────────────────────────────────
    # FUNCIONES HELPER PARA FORMATO DE TEXTO
    # ─────────────────────────────────────────────────────────────────────────
    def add_h1(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(18)
        p.paragraph_format.space_after = Pt(6)
        p.paragraph_format.keep_with_next = True
        r = p.add_run(text)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(15)
        r.font.color.rgb = RGBColor(0x0D, 0x47, 0xA1)
        return p

    def add_h2(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(12)
        p.paragraph_format.space_after = Pt(4)
        p.paragraph_format.keep_with_next = True
        r = p.add_run(text)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(12.5)
        r.font.color.rgb = RGBColor(0x15, 0x65, 0xC0)
        return p

    def add_h3(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(8)
        p.paragraph_format.space_after = Pt(2)
        p.paragraph_format.keep_with_next = True
        r = p.add_run(text)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(11)
        r.font.color.rgb = RGBColor(0x2E, 0x7D, 0x32)
        return p

    def add_p(text, bold_prefix="", italic=False):
        p = doc.add_paragraph()
        p.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(6)
        p.paragraph_format.line_spacing = 1.15
        if bold_prefix:
            rb = p.add_run(bold_prefix)
            rb.bold = True
            rb.font.name = "Calibri"
            rb.font.size = Pt(10.5)
            rb.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
        r = p.add_run(text)
        r.font.name = "Calibri"
        r.font.size = Pt(10.5)
        r.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
        r.italic = italic
        return p

    def add_bullet(text, bold_prefix=""):
        p = doc.add_paragraph(style='List Bullet')
        p.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(3)
        p.paragraph_format.line_spacing = 1.15
        if bold_prefix:
            rb = p.add_run(bold_prefix)
            rb.bold = True
            rb.font.name = "Calibri"
            rb.font.size = Pt(10)
        r = p.add_run(text)
        r.font.name = "Calibri"
        r.font.size = Pt(10)
        return p

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 1: INTRODUCCIÓN Y RESUMEN EJECUTIVO
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("1. Introducción y Resumen Ejecutivo")
    
    add_p(
        "El presente informe detalla el diseño, la fundamentación arquitectónica y el estado de avance de la implementación "
        "del Sistema Integrado de Gestión y Simulación de Entregas Rápidas (SIGER), concebido como el proyecto final del curso "
        "de Tecnología de Objetos en la Universidad Nacional de San Agustín de Arequipa (UNSA).",
        bold_prefix="Contexto y Propósito: "
    )
    
    add_p(
        "En la actualidad, la logística de última milla (last-mile delivery) representa el eslabón más costoso, complejo y crítico "
        "de las cadenas de suministro urbanas. La congestión del tráfico, la diversidad topográfica, la heterogeneidad de los paquetes "
        "y la disponibilidad intermitente de vehículos de reparto exigen sistemas de software con alta capacidad computacional, "
        "sincronización rigurosa de recursos en tiempo real y una arquitectura desacoplada que soporte crecimiento a gran escala."
    )
    
    add_p(
        "SIGER aborda esta problemática mediante una aplicación de escritorio de alto rendimiento desarrollada en C++17 moderno y "
        "el framework gráfico Qt 6.12.0 (compilador MinGW 64-bit y sistema de compilación Ninja/CMake). El sistema integra en una sola "
        "plataforma el registro interactivo de pedidos, la gestión de flota con despacho inteligente, la simulación multihilo paralela "
        "de repartidores y el monitoreo visual en tiempo real a través de un mapa cartográfico de alta definición de Arequipa metropolitana "
        "con 20 nodos logísticos georreferenciados."
    )

    add_callout(
        doc,
        "Cumplimiento Curricular Integral",
        "El sistema SIGER satisface al 100% los seis requisitos temáticos del curso de Tecnología de Objetos:\n"
        "1. POO Avanzada: Clases abstractas puras, polimorfismo dinámico, destructores virtuales y encapsulamiento.\n"
        "2. Programación Genérica y STL: Templates de clase y de funciones miembro, smart pointers (unique, shared, weak) y functors.\n"
        "3. MultiThreading: Hilos dedicados (std::thread), mutexes (std::mutex), monitores condicionales (std::condition_variable) y atomicidad.\n"
        "4. Patrones GoF: Creacional (Factory Method para flota) y Comportamiento (Observer para reactividad modelo-interfaz).\n"
        "5. Arquitectura MVC: Separación estricta entre Model (lógica y simulación), View (GUI Qt6) y Controller (mediadores Qt).\n"
        "6. Interfaz GUI Qt6: Gráficos de analítica (QtCharts), escenas 2D interactivas (QGraphicsScene) y mapa cartográfico incrustado."
    )

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 2: OBJETIVOS DEL PROYECTO
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("2. Objetivos del Proyecto")
    
    add_h2("2.1. Objetivo General")
    add_p(
        "Diseñar, implementar y validar un sistema integral de simulación y monitoreo logístico en tiempo real para paquetería urbana "
        "en Arequipa, aplicando los paradigmas avanzados de Programación Orientada a Objetos, programación genérica con STL moderno, "
        "concurrencia multihilo, patrones de diseño GoF y arquitectura MVC sobre el entorno de desarrollo Qt 6 / C++17."
    )
    
    add_h2("2.2. Objetivos Específicos")
    add_bullet("Modelar una jerarquía polimórfica extensible de vehículos de reparto (Motos, Autos, Camiones) y paquetes de entrega (Estándar, Frágiles), asegurando el cálculo diferenciado de costos, velocidades y tiempos de servicio.", bold_prefix="a) Modelado de Dominio: ")
    add_bullet("Desarrollar estructuras de datos genéricas y thread-safe (Cola<T>, Repositorio<T>, Ruta<CoordType>) utilizando templates y gestión moderna de memoria con punteros inteligentes de la biblioteca estándar (memory).", bold_prefix="b) Programación Genérica: ")
    add_bullet("Implementar un motor de simulación concurrente donde cada repartidor opere en un hilo independiente (std::thread), con mecanismos de sincronización (mutex, condition_variable) que impidan condiciones de carrera y deadlocks.", bold_prefix="c) Concurrencia y Sincronización: ")
    add_bullet("Incorporar los patrones de diseño GoF Factory Method (creación desacoplada de la flota) y Observer (comunicación reactiva desacoplada entre hilos del modelo y la interfaz gráfica).", bold_prefix="d) Patrones de Diseño: ")
    add_bullet("Construir una interfaz gráfica moderna, reactiva e interactiva en Qt 6, compuesta por un Dashboard de monitoreo, gráficos dinámicos con QtCharts y un mapa cartográfico en alta resolución de Arequipa con navegación fluida.", bold_prefix="e) Interfaz Gráfica: ")
    add_bullet("Garantizar la calidad y estabilidad del software mediante una batería automatizada de 75 pruebas unitarias con CMake/CTest, cubriendo el 100% de los módulos críticos.", bold_prefix="f) Verificación y Calidad: ")

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 3: MARCO TEÓRICO Y APLICACIÓN DE TEMAS CURRICULARES
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("3. Marco Teórico y Aplicación de Temas de Tecnología de Objetos")
    
    add_h2("3.1. Programación Orientada a Objetos Avanzada (POO)")
    add_p(
        "La Programación Orientada a Objetos en C++ moderno trasciende la simple agrupación de datos y métodos. "
        "En SIGER, el dominio se modela utilizando clases abstractas que definen contratos formales e interfaces que garantizan "
        "el principio de sustitución de Liskov (LSP)."
    )
    add_bullet(
        "Clase abstracta base en Vehiculo.h. Define tres métodos virtuales puros: calcularCosto(distancia, peso) = 0, "
        "calcularTiempoEntrega(distancia) = 0 y getTipo() = 0. Al poseer al menos una función virtual pura, la clase se vuelve abstracta "
        "e impide instanciaciones erróneas de vehículos sin especialización mecánica.",
        bold_prefix="• Clases Abstractas e Interfaces Puras: "
    )
    add_bullet(
        "Las clases derivadas Moto, Auto y Camion sobreescriben la interfaz polimórfica. Por ejemplo, calcularCosto aplica una tarifa "
        "base por kilómetro y recargos por peso diferenciados (Moto: S/. 0.80/km + recargo si supera 5 kg; Auto: S/. 1.50/km + S/. 0.30/kg; "
        "Camión: S/. 5.00 base + S/. 2.50/km + S/. 0.10/kg). De manera análoga, calcularTiempoEntrega utiliza velocidades nominales distintas "
        "(Moto: 50 km/h, Auto: 40 km/h, Camión: 28 km/h), reflejando el comportamiento urbano real.",
        bold_prefix="• Polimorfismo en Tiempo de Ejecución: "
    )
    add_bullet(
        "La clase Vehiculo y Paquete poseen virtual ~Vehiculo() = default; y virtual ~Paquete() = default;. Esto previene fugas de memoria "
        "cuando se elimina un objeto derivado a través de un puntero a la clase base (std::shared_ptr<Vehiculo>).",
        bold_prefix="• Destructores Virtuales: "
    )

    add_code_block(doc, """// Extracto de Vehiculo.h - Jerarquía Abstracta Polimórfica
class Vehiculo {
public:
    Vehiculo(const std::string& id, const std::string& placa, 
             double capacidadKg, double velocidadKmh);
    virtual ~Vehiculo() = default; // Destructor virtual esencial

    // Métodos virtuales puros (Interfaz polimórfica)
    virtual double calcularCosto(double distanciaKm, double pesoKg) const = 0;
    virtual std::string getTipo() const = 0;
    virtual double calcularTiempoEntrega(double distanciaKm) const = 0;

    // Métodos virtuales con implementación por defecto
    virtual bool puedeCargar(double pesoKg) const {
        return pesoKg > 0.0 && pesoKg <= m_capacidadKg;
    }
protected:
    double m_capacidadKg;
    double m_velocidadKmh;
};""", "C++ - Vehiculo.h")

    add_h2("3.2. Programación Genérica (Templates) y Gestión de Memoria STL")
    add_p(
        "La programación genérica permite escribir algoritmos y estructuras de datos independientes del tipo de dato concreto, "
        "maximizando la reutilización de código y la eficiencia en tiempo de compilación sin incurrir en la penalización de llamadas virtuales."
    )
    add_bullet(
        "template <typename T> class Cola encapsula std::queue<T> bajo un monitor de sincronización std::mutex. "
        "Se diseñaron métodos de orden superior parametrizados con predicados funcionales: "
        "template <typename Predicate> std::optional<T> extraerSi(Predicate pred); y template <typename Predicate> bool contieneSi(Predicate pred) const;. "
        "Esto permite extraer el primer pedido que cumpla una condición arbitraria (por ejemplo, tipo de vehículo y capacidad de carga) "
        "preservando el orden estricto de los demás elementos.",
        bold_prefix="• Plantilla de Clase Concurrente (Cola<T>): "
    )
    add_bullet(
        "template <typename T> class Repositorio implementa un almacenamiento seguro en memoria mediante std::unordered_map<std::string, std::shared_ptr<T>>. "
        "Permite búsquedas y filtrados mediante funciones lambda (std::function<bool(const std::shared_ptr<T>&)>).",
        bold_prefix="• Almacén Genérico (Repositorio<T>): "
    )
    add_bullet(
        "template <typename CoordType> class Ruta implementa la representación geométrica de recorridos y cálculo de distancias "
        "euclidianas tridimensionales o bidimensionales mediante std::sqrt y operadores aritméticos sobre CoordType.",
        bold_prefix="• Geometría Parametrizada (Ruta<CoordType>): "
    )
    add_bullet(
        "Uso intensivo de smart pointers de <memory>. Se prohibió el uso de punteros crudos (raw pointers) con new/delete manual. "
        "std::shared_ptr gestiona la propiedad compartida de pedidos entre cola, repositorio y repartidores concurrentes; "
        "std::unique_ptr transfiere la propiedad exclusiva de vehículos en las factorías; y std::weak_ptr rompe ciclos de referencia en el patrón Observer.",
        bold_prefix="• Gestión Moderna de Memoria: "
    )

    add_code_block(doc, """// Extracto de Cola.h - Método Genérico con Predicado Funcional
template <typename T>
class Cola {
public:
    template <typename Predicate>
    [[nodiscard]] std::optional<T> extraerSi(Predicate pred) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<T> nuevaCola;
        std::optional<T> resultado = std::nullopt;

        while (!m_queue.empty()) {
            T elem = std::move(m_queue.front());
            m_queue.pop();

            if (!resultado.has_value() && pred(elem)) {
                resultado = std::move(elem); // Extrae el primero que coincide
            } else {
                nuevaCola.push(std::move(elem)); // Conserva el orden de los demás
            }
        }
        m_queue = std::move(nuevaCola);
        return resultado;
    }
private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
};""", "C++ - Cola.h")

    add_h2("3.3. Programación Concurrente y MultiThreading")
    add_p(
        "La simulación de entregas en SIGER es un sistema de eventos concurrentes de múltiples productores y múltiples consumidores. "
        "Cada repartidor de la flota es una entidad autónoma ejecutada en su propio hilo de sistema operativo (std::thread)."
    )
    add_bullet(
        "El motor SimuladorEntregas lanza un std::thread por cada Repartidor registrado. Si la flota cuenta con 5 repartidores, "
        "existen 5 hilos de trabajo ejecutándose en paralelo sobre los núcleos físicos de la CPU.",
        bold_prefix="• Modelo de Hilos Independientes: "
    )
    add_bullet(
        "std::condition_variable m_condVar gestiona la sincronización productor-consumidor. Cuando un repartidor no tiene pedidos disponibles "
        "o aptos para su tipo de vehículo, entra en estado de suspensión pasiva con wait_for, liberando el procesador. Al registrarse un nuevo "
        "pedido o al liberarse una unidad, se invoca notify_all(), despertando a los repartidores correspondientes de inmediato.",
        bold_prefix="• Sincronización Reactiva con Variables de Condición: "
    )
    add_bullet(
        "Las variables m_totalEntregados, m_totalFallidos y m_enCamino son de tipo std::atomic<int>. Los flags de control de estado m_corriendo "
        "y m_pausado son std::atomic<bool>, permitiendo lecturas y escrituras lock-free sin penalización de rendimiento.",
        bold_prefix="• Operaciones Atómicas Lock-Free: "
    )
    add_bullet(
        "Se implementó un mutex de despacho (m_despachoMutex) que garantiza que las salidas de los repartidores ocurran de forma escalonada "
        "(con una ventana temporal de ~1.2 segundos), eliminando salidas simultáneas artificiales. Asimismo, el método detener() transfiere "
        "los hilos fuera del mutex principal antes de invocar hilo.join(), previniendo deadlocks durante el cierre del sistema.",
        bold_prefix="• Despacho Escalonado y Prevención de Deadlocks: "
    )

    add_h2("3.4. Patrones de Diseño de Software (GoF)")
    
    add_h3("A. Patrón Creacional: Factory Method")
    add_p(
        "El patrón Factory Method desacopla la creación de los vehículos de su uso en el simulador. La clase abstracta FactoriaVehiculos "
        "define el método virtual puro virtual std::unique_ptr<Vehiculo> crearVehiculo(...) = 0 y mantiene un registro estático "
        "de creadores concretos (MotoFactory, AutoFactory, CamionFactory). El cliente solicita vehículos mediante cadenas ('Moto', 'Auto', 'Camion') "
        "sin conocer las clases derivadas concretas."
    )
    
    add_h3("B. Patrón de Comportamiento: Observer")
    add_p(
        "El patrón Observer es la columna vertebral de la comunicación desacoplada en SIGER. Los sujetos publicadores (GestorPedidos y SimuladorEntregas) "
        "heredan de ISubject y notifican eventos tipados (DatoEvento y EventoSiger). Los suscriptores (PedidoController, SimulacionController, etc.) "
        "implementan la interfaz IObserver con el método virtual void onEvento(const DatoEvento& evento) = 0. "
        "La lista de observadores almacena std::vector<std::weak_ptr<IObserver>>, lo que impide la formación de ciclos de referencia y garantiza "
        "que la destrucción de una vista no deje punteros colgantes (dangling pointers)."
    )

    add_h2("3.5. Arquitectura Modelo - Vista - Controlador (MVC)")
    add_p(
        "El sistema separa taxativamente las responsabilidades en tres capas fundamentales:\n"
        "1. Capa Modelo (src/model/): Código C++ puro sin dependencias gráficas. Aloja las entidades de dominio, el repositorio, las colas genéricas, "
        "el simulador multihilo y los patrones Factory y Observer.\n"
        "2. Capa Controlador (src/controller/): Clases que heredan de QObject e implementan IObserver. PedidoController y SimulacionController "
        "actúan como mediadores bidireccionales: reciben peticiones de la vista (mediante Qt Slots) y las traducen al modelo; y escuchan eventos del modelo "
        "(onEvento) para emitir señales Qt (signals) hacia la interfaz gráfica.\n"
        "3. Capa Vista (src/view/): Interfaces gráficas en Qt 6. Comprende MainWindow, DashboardView, MapaView, EstadisticasView, FormPedido y ControlPanel. "
        "Las vistas solo conocen señales y slots de los controladores, permaneciendo desacopladas del modelo concurrente subyacente."
    )

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 4: COMPONENTES DEL SISTEMA SIGER
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("4. Arquitectura y Componentes del Sistema SIGER")
    
    add_p(
        "La organización modular del código fuente de SIGER se refleja en la siguiente estructura de componentes:",
        bold_prefix="Organización del Código Fuente: "
    )

    t_modulos = doc.add_table(rows=7, cols=3)
    t_modulos.alignment = WD_TABLE_ALIGNMENT.CENTER
    t_modulos.autofit = False
    
    hdrs_mod = ["Capa Arquitectónica", "Archivos Fuente Clave", "Responsabilidad Principal en el Sistema"]
    w_mod = [Inches(1.8), Inches(2.2), Inches(2.5)]
    
    for c_idx, title in enumerate(hdrs_mod):
        cell = t_modulos.cell(0, c_idx)
        cell.width = w_mod[c_idx]
        set_cell_background(cell, "1565C0")
        set_cell_margins(cell, top=100, bottom=100, left=120, right=120)
        p = cell.paragraphs[0]
        r = p.add_run(title)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(9.5)
        r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    datos_mod = [
        ("Modelo (Dominio)", "Vehiculo, Moto, Auto, Camion, Paquete, Pedido, Repartidor", "Entidades polimórficas de negocio, cálculo de costos y tiempos."),
        ("Modelo (Templates)", "Cola<T>, Repositorio<T>, Ruta<T>, Estrategias.h", "Contenedores genéricos, almacén en memoria y functores de asignación."),
        ("Modelo (Concurrencia)", "SimuladorEntregas.cpp, GestorPedidos.cpp", "Gestión de hilos de repartidores, monitores y sincronización atómica."),
        ("Modelo (Patrones)", "VehiculoFactory.cpp, Observer.h", "Implementación de Factory Method y publicador/suscriptor Observer."),
        ("Controlador (MVC)", "PedidoController.cpp, SimulacionController.cpp", "Mediadores entre eventos del modelo e interfaz gráfica de Qt."),
        ("Vista (Qt 6 GUI)", "MainWindow, DashboardView, MapaView, EstadisticasView, etc.", "Presentación interactiva, escenas QGraphicsScene y gráficos QtCharts.")
    ]

    for r_idx, (capa, archs, resp) in enumerate(datos_mod, start=1):
        bg = "F5F7FA" if r_idx % 2 == 1 else "FFFFFF"
        for c_idx, val in enumerate([capa, archs, resp]):
            cell = t_modulos.cell(r_idx, c_idx)
            cell.width = w_mod[c_idx]
            set_cell_background(cell, bg)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            r = p.add_run(val)
            r.font.name = "Calibri"
            r.font.size = Pt(8.5)
            if c_idx == 0:
                r.bold = True

    p_sp = doc.add_paragraph()
    p_sp.paragraph_format.space_before = Pt(6)

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 5: LÓGICA DE FLOTA Y SIMULACIÓN INTELIGENTE
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("5. Mecanismo de Simulación y Lógica de Flota")
    
    add_h2("5.1. Composición de la Flota Logística")
    add_p(
        "En el ensamblado del sistema (main.cpp), se inicializa una flota vehicular balanceada de 5 repartidores "
        "creados mediante FactoriaVehiculos::crear:"
    )
    add_bullet("R-101 (Carlos Quispe - Moto 1): Vehículo Moto V-01 (MTO-501), capacidad máxima de 15.0 kg.", bold_prefix="• Moto 1: ")
    add_bullet("R-102 (Lucía Torres - Moto 2): Vehículo Moto V-02 (MTO-502), capacidad máxima de 15.0 kg.", bold_prefix="• Moto 2: ")
    add_bullet("R-103 (María Flores - Auto 1): Vehículo Auto V-03 (AUT-701), capacidad máxima de 100.0 kg.", bold_prefix="• Auto 1: ")
    add_bullet("R-104 (Jorge Ramos - Auto 2): Vehículo Auto V-04 (AUT-702), capacidad máxima de 100.0 kg.", bold_prefix="• Auto 2: ")
    add_bullet("R-105 (Pedro Mamani - Camión): Vehículo Camión V-05 (CAM-901), capacidad máxima de 2000.0 kg.", bold_prefix="• Camión 1: ")

    add_h2("5.2. Asignación Exclusiva por Tipo y Espera Reactiva")
    add_p(
        "Cada pedido registrado por el usuario almacena su tipo de vehículo requerido (m_tipoVehiculoRequerido), deducido automáticamente "
        "del peso del paquete o seleccionado manualmente en el formulario (FormPedido). La regla operativa fundamental implementada establece que:\n"
        "• Un pedido que requiere 'Moto' únicamente puede ser atendido por una Moto.\n"
        "• Un pedido que requiere 'Auto' únicamente puede ser atendido por un Auto.\n"
        "• Un pedido que requiere 'Camión' únicamente puede ser atendido por un Camión."
    )
    add_p(
        "Si el usuario registra, por ejemplo, tres pedidos sucesivos que requieren Moto y las dos motos de la flota ya se encuentran "
        "en reparto (estado EN_REPARTO), el tercer pedido permanece en espera en la cola compartida. Ni los autos ni el camión toman "
        "el pedido indebidamente. Cuando cualquiera de las dos motos completa su recorrido y regresa a estado LIBRE, notifica a la variable "
        "de condición (m_condVar.notify_all()), toma el pedido en espera y sale de inmediato a repartir. Este comportamiento modela con fidelidad "
        "la gestión real de recursos vehiculares en empresas de mensajería."
    )

    add_h2("5.3. Tiempos de Recorrido Calibrados y Animación Sincronizada")
    add_p(
        "Para permitir una inspección visual agradable y comprensible del flujo logístico, la velocidad del simulador multihilo fue calibrada "
        "en estrecha sincronía con la velocidad de renderizado en MapaView:"
    )
    add_bullet("Motos (Ligeras): Recorrido animado de ~5.5 a 6.0 segundos en color celeste brillante (#0288D1) con icono 🏍️.", bold_prefix="• ")
    add_bullet("Autos (Intermedios): Recorrido animado de ~7.5 a 8.0 segundos en color naranja ámbar (#FB8C00) con icono 🚗.", bold_prefix="• ")
    add_bullet("Camiones (Pesados): Recorrido animado de ~10.0 a 11.0 segundos en color púrpura (#8E24AA) con icono 🚚.", bold_prefix="• ")
    add_p(
        "El móvil llega a su nodo de destino exactamente en el mismo instante en que el hilo repartidor despierta y notifica el evento PEDIDO_COMPLETADO, "
        "logrando una sincronía milimétrica entre la física del modelo y la vista 2D."
    )

    add_h2("5.4. Finalización Automática de Simulación")
    add_p(
        "Cuando el total de pedidos completados más pedidos cancelados alcanza la cantidad total de pedidos registrados en el sistema, "
        "GestorPedidos emite automáticamente el evento EventoSiger::TODOS_COMPLETADOS. SimulacionController captura este evento, detiene limpiamente "
        "los hilos de simulación en segundo plano y MainWindow presenta un diálogo de confirmación informando que la entrega del lote ha culminado "
        "satisfactoriamente, congelando las métricas en su estado final."
    )

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 6: MAPA METROPOLITANO DE AREQUIPA
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("6. Mapa Metropolitano de Arequipa y Georreferenciación")
    
    add_h2("6.1. Integración Cartográfica en Alta Definición")
    add_p(
        "Para proporcionar un contexto geográfico real y profesional, se integró un mapa cartográfico urbano en alta resolución (1536 x 1536 píxeles) "
        "proveniente de mosaicos oficiales de OpenStreetMap con estética visual idéntica a Google Maps. La imagen abarca desde el Parque Industrial "
        "y Río Seco por el norte, hasta Sabandía y Socabaya por el sur, y desde Sachaca por el oeste hasta Paucarpata por el este. "
        "El mapa fue empaquetado directamente en el ejecutable SIGER.exe a través del sistema de recursos de Qt (resources.qrc), "
        "garantizando funcionamiento 100% autónomo y offline sin depender de conexión a internet."
    )

    add_h2("6.2. Proyección Matemática Web Mercator")
    add_p(
        "La traslación de coordenadas geográficas GPS (latitud y longitud en grados decimales) a coordenadas cartesianas de la escena "
        "QGraphicsScene (-768 a +768 px) se realiza mediante la proyección matemática estándar Web Mercator a nivel de zoom 14:"
    )
    add_p(
        "x_tile = ((lon + 180) / 360) * 2^14\n"
        "y_tile = (1 - asinh(tan(rad(lat))) / pi) / 2 * 2^14\n"
        "x_pixel = (x_tile - x_min_tile) * 256 - ancho_escena / 2\n"
        "y_pixel = (y_tile - y_min_tile) * 256 - alto_escena / 2",
        italic=True
    )
    add_p(
        "Gracias a esta parametrización, los 20 nodos logísticos caen con precisión sobre sus ubicaciones reales (avenidas, plazas, campus y centros comerciales)."
    )

    # Si existe la imagen del mapa, incluir una miniatura
    ruta_img_mapa = "resources/mapa_arequipa.png"
    if os.path.exists(ruta_img_mapa):
        try:
            p_img = doc.add_paragraph()
            p_img.alignment = WD_ALIGN_PARAGRAPH.CENTER
            p_img.paragraph_format.space_before = Pt(8)
            p_img.paragraph_format.space_after = Pt(4)
            run_img = p_img.add_run()
            run_img.add_picture(ruta_img_mapa, width=Inches(5.0))
            p_caption = doc.add_paragraph()
            p_caption.alignment = WD_ALIGN_PARAGRAPH.CENTER
            p_caption.paragraph_format.space_before = Pt(0)
            p_caption.paragraph_format.space_after = Pt(12)
            r_cap = p_caption.add_run("Figura 1: Cartografía urbana de Arequipa metropolitana integrada en MapaView.")
            r_cap.font.name = "Calibri"
            r_cap.font.size = Pt(8.5)
            r_cap.italic = True
        except Exception as e:
            print("No se pudo insertar miniatura del mapa:", e)

    add_h2("6.3. Catálogo de los 20 Nodos Logísticos Metropolitanos")
    add_p("A continuación se presenta el catálogo completo de las 20 ubicaciones metropolitanas incorporadas en el sistema:")

    t_nodos = doc.add_table(rows=21, cols=4)
    t_nodos.alignment = WD_TABLE_ALIGNMENT.CENTER
    t_nodos.autofit = False
    
    hdrs_nod = ["#", "Ubicación / Nodo Logístico", "Distrito Metropolitano", "Coordenadas Escena (X, Y)"]
    w_nod = [Inches(0.5), Inches(2.7), Inches(1.8), Inches(1.5)]
    
    for c_idx, title in enumerate(hdrs_nod):
        cell = t_nodos.cell(0, c_idx)
        cell.width = w_nod[c_idx]
        set_cell_background(cell, "0D47A1")
        set_cell_margins(cell, top=80, bottom=80, left=100, right=100)
        p = cell.paragraphs[0]
        r = p.add_run(title)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(8.5)
        r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    catalogo_nodos = [
        ("1", "Plaza de Armas", "Arequipa (Cercado)", "(70.7, -68.9)"),
        ("2", "Calle Mercaderes 120", "Arequipa (Cercado)", "(95.2, -78.6)"),
        ("3", "Calle San Francisco 204", "Arequipa (Cercado)", "(92.8, -105.3)"),
        ("4", "Mirador de Yanahuara", "Yanahuara", "(9.0, -197.6)"),
        ("5", "Av. Ejercito 450 (Mall Plaza)", "Cayma / Yanahuara", "(-17.8, -161.2)"),
        ("6", "Av. Cayma 310", "Cayma", "(-52.8, -254.7)"),
        ("7", "Urb. Umacollo B-12", "Yanahuara", "(-47.0, -23.9)"),
        ("8", "UNSA Ingenierias (Av. Independencia)", "Arequipa (Cercado)", "(174.4, 8.9)"),
        ("9", "Av. Venezuela 800 (UNSA)", "Arequipa (Cercado)", "(250.1, 61.1)"),
        ("10", "Av. Lambramani 105 (Parque Lambramani)", "JLByR", "(246.6, 142.5)"),
        ("11", "Mall Aventura Porongoche", "Paucarpata", "(355.0, 259.1)"),
        ("12", "Av. Dolores 890", "JLByR", "(211.7, 206.8)"),
        ("13", "Plaza Mayta Capac (Miraflores)", "Miraflores", "(290.9, -145.4)"),
        ("14", "Parque Selva Alegre", "Alto Selva Alegre", "(133.6, -206.1)"),
        ("15", "Mirador de Sachaca", "Sachaca", "(-250.9, 251.8)"),
        ("16", "Terminal Terrestre Arequipa", "Jacobo Hunter", "(-72.6, 279.7)"),
        ("17", "Cerro Colorado (Plaza Las Americas)", "Cerro Colorado", "(-196.1, -473.3)"),
        ("18", "Parque Industrial", "Cerro Colorado / Río Seco", "(-483.9, -603.2)"),
        ("19", "Plaza de Paucarpata", "Paucarpata", "(518.1, 344.1)"),
        ("20", "Molino de Sabandia", "Sabandía", "(640.4, 577.3)")
    ]

    for r_idx, (num, nom, dist, coords) in enumerate(catalogo_nodos, start=1):
        bg = "F5F7FA" if r_idx % 2 == 1 else "FFFFFF"
        for c_idx, val in enumerate([num, nom, dist, coords]):
            cell = t_nodos.cell(r_idx, c_idx)
            cell.width = w_nod[c_idx]
            set_cell_background(cell, bg)
            set_cell_margins(cell, top=60, bottom=60, left=80, right=80)
            p = cell.paragraphs[0]
            r = p.add_run(val)
            r.font.name = "Calibri"
            r.font.size = Pt(8.0)
            if c_idx == 1:
                r.bold = True

    add_h2("6.4. Navegación e Interacción Tipo Google Maps")
    add_p(
        "Para ofrecer una experiencia inmersiva, MapaView incluye controles de navegación de nivel industrial:\n"
        "• Arrastre con Mano (ScrollHandDrag): Permite hacer clic y arrastrar suavemente el mapa en cualquier dirección.\n"
        "• Zoom Dinámico con Rueda del Ratón (WheelEvent): Escala el mapa suavemente centrado en el puntero del mouse.\n"
        "• Controles en Barra Superior: Botones [🔍 +] para acercar, [🔍 -] para alejar y [🎯 Centrar] para reenfocar instantáneamente la vista en el Centro Histórico de Arequipa.\n"
        "• Pines con Etiquetas Pill: Cada nodo cuenta con un pin circular con aura de resplandor (rojo para puntos urbanos y azul para la UNSA/Centro) y una etiqueta estilizada con fondo blanco semitransparente de alto contraste."
    )

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 7: CALIDAD DE SOFTWARE Y PRUEBAS UNITARIAS
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("7. Calidad de Software y Verificación (Unit Testing)")
    add_p(
        "Para garantizar la robustez matemática, la integridad de memoria y la ausencia de condiciones de carrera, "
        "se implementó una batería de 75 pruebas unitarias automatizadas agrupadas en tres ejecutables de test CTest:"
    )

    t_tests = doc.add_table(rows=4, cols=4)
    t_tests.alignment = WD_TABLE_ALIGNMENT.CENTER
    t_tests.autofit = False
    
    hdrs_tst = ["Suite de Prueba", "Módulo y Objetivos Evaluados", "Aserciones / Casos", "Tasa de Éxito"]
    w_tst = [Inches(1.5), Inches(2.8), Inches(1.1), Inches(1.1)]
    
    for c_idx, title in enumerate(hdrs_tst):
        cell = t_tests.cell(0, c_idx)
        cell.width = w_tst[c_idx]
        set_cell_background(cell, "2E7D32") # Verde calidad
        set_cell_margins(cell, top=80, bottom=80, left=100, right=100)
        p = cell.paragraphs[0]
        r = p.add_run(title)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(8.5)
        r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    datos_tests = [
        ("test_dominio", "Jerarquía Vehiculo/Paquete, cálculo polimórfico de costos, tiempos de servicio y capacidades de carga.", "25 aserciones", "100% PASSED"),
        ("test_generico", "Cola<T> FIFO, Repositorio<T>, Ruta<CoordType>, functors de estrategia y filtrado lambda.", "25 aserciones", "100% PASSED"),
        ("test_threading", "Concurrencia multihilo, sincronización productor-consumidor con condition_variable, atomicidad y parada limpia.", "25 aserciones", "100% PASSED")
    ]

    for r_idx, (suite, desc, casos, res) in enumerate(datos_tests, start=1):
        bg = "F5F7FA" if r_idx % 2 == 1 else "FFFFFF"
        for c_idx, val in enumerate([suite, desc, casos, res]):
            cell = t_tests.cell(r_idx, c_idx)
            cell.width = w_tst[c_idx]
            set_cell_background(cell, bg)
            set_cell_margins(cell, top=80, bottom=80, left=100, right=100)
            p = cell.paragraphs[0]
            r = p.add_run(val)
            r.font.name = "Calibri"
            r.font.size = Pt(8.5)
            if c_idx == 0:
                r.bold = True
            elif c_idx == 3:
                r.bold = True
                r.font.color.rgb = RGBColor(0x2E, 0x7D, 0x32)

    add_p(
        "Métrica Global: 100% de pruebas aprobadas (75/75 exitosas, 0 fallos). Tiempo total de ejecución: ~0.48 segundos.",
        bold_prefix="Resultado Global: "
    )

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 8: TRAZABILIDAD EN GIT Y REPOSITORIO GITHUB
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("8. Trazabilidad de Trabajo y Control de Versiones (Git / GitHub)")
    add_p(
        "El desarrollo del proyecto se gestionó bajo buenas prácticas de control de versiones con Git, aplicando la convención "
        "Conventional Commits (feat, fix, docs, chore). Los commits se distribuyeron equitativamente entre los cuatro integrantes del equipo "
        "según sus áreas de especialidad curricular. Todos los cambios se encuentran sincronizados en la rama principal (main) del repositorio remoto:"
    )

    t_git = doc.add_table(rows=12, cols=3)
    t_git.alignment = WD_TABLE_ALIGNMENT.CENTER
    t_git.autofit = False
    
    hdrs_git = ["Commit Hash", "Autor / Correo Institucional", "Mensaje del Commit y Alcance Técnico"]
    w_git = [Inches(1.2), Inches(2.2), Inches(3.1)]
    
    for c_idx, title in enumerate(hdrs_git):
        cell = t_git.cell(0, c_idx)
        cell.width = w_git[c_idx]
        set_cell_background(cell, "37474F") # Gris oscuro
        set_cell_margins(cell, top=80, bottom=80, left=100, right=100)
        p = cell.paragraphs[0]
        r = p.add_run(title)
        r.bold = True
        r.font.name = "Calibri"
        r.font.size = Pt(8.5)
        r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    datos_git = [
        ("115f730", "R. Poncel <rponcel@unsa.edu.pe>", "docs: informe formal de avance del proyecto final SIGER"),
        ("e7d8e7f", "L. Baca <lbacac@unsa.edu.pe>", "feat(mapa): mapa urbano de Arequipa estilo Google Maps con 20 nodos"),
        ("19983c7", "German Chipana <gchipanaj@unsa.edu.pe>", "feat(gui): visualizacion sincronizada de flota y animacion pausada de rutas"),
        ("67f31c1", "G. Mejia <gmejiar@unsa.edu.pe>", "feat(core): asignacion de flota por tipo de vehiculo y despacho escalonado"),
        ("30aeed6", "R. Poncel <rponcel@unsa.edu.pe>", "feat(sim): simular solo pedidos manuales y detener automaticamente al completar"),
        ("617b1b3", "G. Mejia <gmejiar@unsa.edu.pe>", "fix(stats): corregir contador de completados y tasa de exito en estadisticas"),
        ("a78e6c1", "German Chipana <gchipanaj@unsa.edu.pe>", "fix(core): corregir sincronizacion de hilos en simulador y mapa 2D"),
        ("3300475", "ChipanaGerman <gchipanaj@unsa.edu.pe>", "Update team information in README"),
        ("609cacd", "German Chipana <gchipanaj@unsa.edu.pe>", "feat(mvc-gui): controladores MVC, interfaz grafica en Qt6 y aplicacion principal"),
        ("3eee1d0", "R. Poncel <rponcel@unsa.edu.pe>", "feat(threading): multithreading, mutex, condicion, generador de eventos y patrones GoF"),
        ("6a47a91", "G. Mejia <gmejiar@unsa.edu.pe>", "feat(generico): templates Cola<T>, Repositorio<T>, Ruta<Coord> y memoria STL")
    ]

    for r_idx, (hsh, autor, msg) in enumerate(datos_git, start=1):
        bg = "F5F7FA" if r_idx % 2 == 1 else "FFFFFF"
        for c_idx, val in enumerate([hsh, autor, msg]):
            cell = t_git.cell(r_idx, c_idx)
            cell.width = w_git[c_idx]
            set_cell_background(cell, bg)
            set_cell_margins(cell, top=60, bottom=60, left=80, right=80)
            p = cell.paragraphs[0]
            r = p.add_run(val)
            r.font.name = "Calibri"
            r.font.size = Pt(8.0)
            if c_idx == 0:
                r.font.name = "Consolas"
                r.bold = True
            elif c_idx == 1:
                r.bold = True

    # ─────────────────────────────────────────────────────────────────────────
    # SECCIÓN 9: CONCLUSIONES Y TRABAJO FUTURO
    # ─────────────────────────────────────────────────────────────────────────
    add_h1("9. Conclusiones y Trabajo Futuro")
    
    add_h2("9.1. Conclusiones")
    add_bullet(
        "Se implementó con éxito una solución de software robusta, moderna y eficiente en C++17 y Qt 6 que satisface con rigor "
        "académico todos los requisitos temáticos del curso de Tecnología de Objetos (UNSA).",
        bold_prefix="1. Cumplimiento Curricular Pleno: "
    )
    add_bullet(
        "El uso de programación genérica combinada con smart pointers y lambdas permitió crear contenedores concurrentes desacoplados "
        "y seguros, libres de fugas de memoria o punteros colgantes.",
        bold_prefix="2. Seguridad de Memoria y Rendimiento: "
    )
    add_bullet(
        "El motor multihilo modela con fidelidad la física y la logística de última milla, combinando espera reactiva sin consumo inútil "
        "de CPU con un despacho ordenado que previene condiciones de carrera y deadlocks.",
        bold_prefix="3. Concurrencia de Nivel Industrial: "
    )
    add_bullet(
        "La interfaz en Qt 6 ofrece una experiencia interactiva realista y contextualizada en Arequipa metropolitana, combinando "
        "cartografía de alta definición, analítica visual con QtCharts y manipulación intuitiva con ratón.",
        bold_prefix="4. Experiencia Visual de Alto Impacto: "
    )

    add_h2("9.2. Trabajo Futuro y Líneas de Expansión")
    add_bullet("Incorporar algoritmos de enrutamiento óptimo sobre grafos (Dijkstra y A*) sobre las aristas viales reales de Arequipa.", bold_prefix="• Optimización de Rutas: ")
    add_bullet("Implementar un módulo de persistencia con SQLite para almacenar el historial de pedidos y auditoría de tiempos de entrega.", bold_prefix="• Persistencia Relacional: ")
    add_bullet("Añadir factores climáticos y congestión vehicular dinámica por horarios punta que modifiquen las velocidades de los vehículos en tiempo real.", bold_prefix="• Simulación de Tráfico Dinámico: ")

    # Guardar documento
    output_path = "SIGER_InformeDeAvance.docx"
    doc.save(output_path)
    print(f"Documento Word generado exitosamente: {output_path}")

if __name__ == "__main__":
    main()
