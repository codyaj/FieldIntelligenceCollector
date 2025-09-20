import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-settings',
  imports: [CommonModule, FormsModule],
  templateUrl: './settings.html',
  styleUrl: './settings.scss'
})
export class Settings {
  activePage: string = 'wifi';

  wifiChannels = Array.from({ length: 13 }, (_, i) => ({
    label: `CH ${i + 1}`,
    selected: true
  }));

  hopSpeed = 200;

  usedStorage = 120;  // fetch from backend
  totalStorage = 500;

  setActivePage(page: string) {
    this.activePage = page;
  }

  saveWifiConfig() {
    console.log('Saved WiFi config:', {
      channels: this.wifiChannels.filter(c => c.selected).map(c => c.label),
      hopSpeed: this.hopSpeed
    });
  }

  exportData() {
    console.log('Exporting DB data...');
    // call backend API
  }

  purgeData() {
    if (confirm('Are you sure? This will permanently delete all data.')) {
      console.log('Purging DB...');
      // call backend API
    }
  }
}
