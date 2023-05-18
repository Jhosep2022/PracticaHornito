import mysql.connector
import tkinter as tk
from tkinter import ttk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import datetime
import numpy as np


def get_data():
    conn = mysql.connector.connect(user='root',
                                   password='',
                                   host='localhost',
                                   port=3306,
                                   database='PRACTICA8db')
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM TablaHorno')
    data = cursor.fetchall()
    return data


def plot_data(data):

    fig = Figure(figsize=(5, 5))
    a = fig.add_subplot(111)
    sensor = [i[9] for i in data]
    set_point = [i[8] for i in data]
    calefactor = [i[6] for i in data]
    enfriador = [i[7] for i in data]
    hora = [i[4].total_seconds() for i in data if i[4] is not None]
    a.plot(hora, sensor, label='SensorLM35')
    a.plot(hora, set_point, label='Set_Point')
    a.plot(hora, calefactor, label='Calefactor')
    a.plot(hora, enfriador, label='Enfriador')
    a.legend()
    return fig


def calculate_time(data):
    estado = [i[5] for i in data]
    encendido = estado.count(1)
    apagado = estado.count(0)
    total = len(estado)
    return encendido / total * 100, apagado / total * 100


def calculate_energy(data):
    encendido = calculate_time(data)[0]
    energy = encendido / 60 * 1.0  # asumir que el horno consume 1kWh cuando está encendido
    return energy

class App:
    def __init__(self, root):
        self.root = root
        self.tree = ttk.Treeview(root)
        self.tree["columns"]=("NumReg", "Horno_Id", "Zona", "Fecha", "HoraActual", "Estado_del_Horno", "Calefactor", "Enfriador", "Set_Point", "TempProceso", "SensorLM35")
        for column in self.tree["columns"]:
            self.tree.column(column, width=100)
            self.tree.heading(column, text=column)
        self.tree.pack()
        self.button = tk.Button(root, text="Actualizar datos", command=self.update)
        self.button.pack()
        self.fig = plot_data(get_data())
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        encendido, apagado = calculate_time(get_data())
        energy = calculate_energy(get_data())
        self.label1 = tk.Label(root, text=f'Tiempo encendido: {encendido}%')
        self.label1.pack()
        self.label2 = tk.Label(root, text=f'Tiempo apagado: {apagado}%')
        self.label2.pack()
        self.label3 = tk.Label(root, text=f'Consumo energético: {energy} kWh')
        self.label3.pack()

    def update(self):
        for i in self.tree.get_children():
            self.tree.delete(i)
        data = get_data()
        for row in data:
            self.tree.insert("", "end", values=row)
        self.fig = plot_data(data)
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.draw()
        encendido, apagado = calculate_time(data)
        energy = calculate_energy(data)
        self.label1.configure(text=f'Tiempo encendido: {encendido}%')
        self.label2.configure(text=f'Tiempo apagado: {apagado}%')
        self.label3.configure(text=f'Consumo energético: {energy} kWh')


root = tk.Tk()
app = App(root)
root.mainloop()
