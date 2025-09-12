import { Component, ViewChild, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import {
  NgApexchartsModule,
  ChartComponent,
  ApexAxisChartSeries,
  ApexChart,
  ApexXAxis,
  ApexStroke,
  ApexTooltip,
  ApexLegend,
  ApexNonAxisChartSeries,
  ApexResponsive
} from 'ng-apexcharts';

export type ChartOptions = {
  series: ApexAxisChartSeries;
  chart: ApexChart;
  xaxis: ApexXAxis;
  stroke: ApexStroke;
  tooltip: ApexTooltip;
  legend: ApexLegend;
};

export type PieChartOptions = {
  series: ApexNonAxisChartSeries;
  chart: ApexChart;
  labels: string[];
  tooltip: ApexTooltip;
  legend: ApexLegend;
  responsive?: ApexResponsive[];
};

@Component({
  selector: 'app-livedata',
  standalone: true,
  imports: [CommonModule, NgApexchartsModule],
  templateUrl: './livedata.html',
  styleUrls: ['./livedata.scss']
})
export class Livedata implements OnInit {
  @ViewChild("chart") chart!: ChartComponent;
  public chartOptions: ChartOptions;
  public deviceChartOptions: PieChartOptions;
  logs: string[] = [
    "[Device Recognized] New x device found with 92% confidence!",
    "[SSID Found] New SSID found: name1",
    "[SSID Found] New SSID found: name2",
    "[SSID Found] New SSID found: name3"
  ];

  constructor() {
    // Import channels from backend
    const channels: ApexAxisChartSeries = Array.from({ length: 50 }, (_, i) => ({
      name: `Channel ${i + 1}`,
      data: Array.from({ length: 30 }, () => 0)
    }));

    this.chartOptions = {
      series: channels,
      chart: {
        type: "line",
        height: 570,
        animations: {
          enabled: true,
          dynamicAnimation: { enabled: true, speed: 250 }
        },
        foreColor: '#F8F9FA'
      },
      stroke: { curve: 'smooth' },
      xaxis: { categories: Array.from({ length: 30 }, (_, i) => i) },
      tooltip: { enabled: true, theme: 'dark' },
      legend: { show: true }
    };

    this.deviceChartOptions = {
      series: [],
      chart: {
        type: 'pie',
        height: 270,
        foreColor: '#F8F9FA'
      },
      labels: ['Camera', 'Light', 'Sensor'],
      tooltip: { enabled: true, theme: 'dark' },
      legend: { show: true }
    };
  }

  ngOnInit() {
    this.updateDeviceData([12, 3, 5, 2], ['Camera', 'Light', 'Sensor', 'Speaker']);
    setInterval(() => {
      // Update loop
      //const newValues = Array.from({ length: 50 }, () => Math.floor(Math.random() * 100));
      //this.updateData(newValues);
    }, 250);
  }

  updateData(newValues: number[]) {
    this.chartOptions.series = this.chartOptions.series.map((s, idx) => ({
      name: s.name,
      data: [...(s.data as number[]).slice(1), newValues[idx]]
    }));
  }

  updateDeviceData(newCounts: number[], newLabels: string[]) {
    this.deviceChartOptions.series = newCounts;
    this.deviceChartOptions.labels = newLabels;
  }  
}
