import { Component, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import {
  NgApexchartsModule,
  ApexAxisChartSeries,
  ApexChart,
  ApexXAxis,
  ApexStroke,
  ApexTooltip,
  ApexLegend,
  ApexPlotOptions
} from 'ng-apexcharts';

export type BarChartOptions = {
  series: ApexAxisChartSeries;
  chart: ApexChart;
  xaxis: ApexXAxis;
  tooltip: ApexTooltip;
  legend: ApexLegend;
  plotOptions: ApexPlotOptions;
};

export type LineChartOptions = {
  series: ApexAxisChartSeries;
  chart: ApexChart;
  xaxis: ApexXAxis;
  stroke: ApexStroke;
  tooltip: ApexTooltip;
  legend: ApexLegend;
};

@Component({
  selector: 'app-analytics',
  imports: [CommonModule, FormsModule, NgApexchartsModule],
  templateUrl: './analytics.html',
  styleUrl: './analytics.scss'
})
export class Analytics implements OnInit {
  ssids: string[] = ['SSID-1', 'SSID-2', 'SSID-3'];
  selectedSSID: string = this.ssids[0];

  public ouiChartOptions: BarChartOptions;
  public protocolChartOptions: BarChartOptions;
  public timelineChartOptions: LineChartOptions;

  constructor() {
    this.ouiChartOptions = {
      series: [
        { name: 'Vendors', data: [44, 55, 41, 67, 22, 43] }
      ],
      chart: { type: 'bar', height: 350, foreColor: '#F8F9FA' },
      xaxis: { categories: ['Apple', 'Samsung', 'Intel', 'Cisco', 'Huawei', 'Other'] },
      plotOptions: { bar: { horizontal: false, columnWidth: '60%' } },
      tooltip: { enabled: true, theme: 'dark' },
      legend: { show: false }
    };

    this.protocolChartOptions = {
      series: [
        { name: 'Protocols', data: [30, 20, 15, 10, 25] }
      ],
      chart: { type: 'bar', height: 350, foreColor: '#F8F9FA' },
      xaxis: { categories: ['TCP', 'UDP', 'HTTP', 'HTTPS', 'Other'] },
      plotOptions: { bar: { horizontal: false, columnWidth: '60%' } },
      tooltip: { enabled: true, theme: 'dark' },
      legend: { show: false }
    };

    this.timelineChartOptions = {
      series: [
        { name: 'Devices', data: [10, 12, 15, 20, 25, 30, 28] },
        { name: 'WiFi Networks', data: [5, 8, 9, 12, 15, 18, 17] }
      ],
      chart: { type: 'line', height: 400, foreColor: '#F8F9FA', animations: { enabled: true } },
      stroke: { curve: 'smooth' },
      xaxis: { categories: ['T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7'] },
      tooltip: { enabled: true, theme: 'dark' },
      legend: { show: true }
    };
  }

  ngOnInit() {
    this.loadSSIDData(this.selectedSSID);
  }

  onSSIDChange() {
    this.loadSSIDData(this.selectedSSID);
  }

  loadSSIDData(ssid: string) {
    // Fetch from backend
    if (ssid === 'SSID-1') {
      this.ouiChartOptions.series = [{ name: 'Vendors', data: [44, 55, 41, 67, 22, 43] }];
      this.protocolChartOptions.series = [{ name: 'Protocols', data: [30, 20, 15, 10, 25] }];
    } else if (ssid === 'SSID-2') {
      this.ouiChartOptions.series = [{ name: 'Vendors', data: [10, 40, 32, 55, 18, 25] }];
      this.protocolChartOptions.series = [{ name: 'Protocols', data: [25, 15, 35, 5, 20] }];
    } else {
      this.ouiChartOptions.series = [{ name: 'Vendors', data: [20, 10, 15, 5, 8, 12] }];
      this.protocolChartOptions.series = [{ name: 'Protocols', data: [15, 5, 25, 20, 10] }];
    }
  }
}
