import { ComponentFixture, TestBed } from '@angular/core/testing';

import { InterruptDescriptorTableComponent } from './interrupt-descriptor-table.component';

describe('InterruptDescriptorTableComponent', () => {
  let component: InterruptDescriptorTableComponent;
  let fixture: ComponentFixture<InterruptDescriptorTableComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ InterruptDescriptorTableComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(InterruptDescriptorTableComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
