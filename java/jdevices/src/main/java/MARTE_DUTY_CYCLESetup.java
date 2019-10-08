/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author manduchi
 */
public class MARTE_DUTY_CYCLESetup extends DeviceSetup {

    /**
     * Creates new form MARTE_DUDTY_CYCLESetup
     */
    public MARTE_DUTY_CYCLESetup() {
	initComponents();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

	deviceButtons1 = new DeviceButtons();
	jPanel1 = new javax.swing.JPanel();
	jPanel2 = new javax.swing.JPanel();
	deviceField1 = new DeviceField();
	deviceDispatch1 = new DeviceDispatch();
	jPanel3 = new javax.swing.JPanel();
	deviceField2 = new DeviceField();
	deviceField3 = new DeviceField();
	deviceField4 = new DeviceField();
	deviceField5 = new DeviceField();
	jPanel4 = new javax.swing.JPanel();
	deviceField6 = new DeviceField();
	deviceField7 = new DeviceField();
	deviceField8 = new DeviceField();

	setDeviceProvider("localhost");
	setDeviceTitle("Variable Dudt Cycle clock generator");
	setDeviceType("MARTE_DUTY_CYCLE");
	setHeight(250);
	setWidth(500);
	getContentPane().add(deviceButtons1, java.awt.BorderLayout.SOUTH);

	jPanel1.setLayout(new java.awt.GridLayout(3, 1));

	deviceField1.setIdentifier("");
	deviceField1.setLabelString("Comment:");
	deviceField1.setNumCols(25);
	deviceField1.setOffsetNid(1);
	deviceField1.setTextOnly(true);
	jPanel2.add(deviceField1);
	jPanel2.add(deviceDispatch1);

	jPanel1.add(jPanel2);

	deviceField2.setIdentifier("");
	deviceField2.setLabelString("Device ID: ");
	deviceField2.setNumCols(3);
	deviceField2.setOffsetNid(2);
	jPanel3.add(deviceField2);

	deviceField3.setIdentifier("");
	deviceField3.setLabelString("In Clock bit:");
	deviceField3.setNumCols(3);
	deviceField3.setOffsetNid(1039);
	jPanel3.add(deviceField3);

	deviceField4.setIdentifier("");
	deviceField4.setLabelString("Enable bit:");
	deviceField4.setNumCols(3);
	deviceField4.setOffsetNid(1045);
	jPanel3.add(deviceField4);

	deviceField5.setIdentifier("");
	deviceField5.setLabelString("Out Clock bit:");
	deviceField5.setNumCols(3);
	deviceField5.setOffsetNid(1051);
	jPanel3.add(deviceField5);

	jPanel1.add(jPanel3);

	deviceField6.setIdentifier("");
	deviceField6.setLabelString("Input Freq.(Hz): ");
	deviceField6.setNumCols(4);
	deviceField6.setOffsetNid(1057);
	jPanel4.add(deviceField6);

	deviceField7.setIdentifier("");
	deviceField7.setLabelString("Output Freq. (Hz):");
	deviceField7.setNumCols(4);
	deviceField7.setOffsetNid(1063);
	jPanel4.add(deviceField7);

	deviceField8.setIdentifier("");
	deviceField8.setLabelString("Duty Cycle %:");
	deviceField8.setNumCols(3);
	deviceField8.setOffsetNid(1069);
	jPanel4.add(deviceField8);

	jPanel1.add(jPanel4);

	getContentPane().add(jPanel1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceDispatch deviceDispatch1;
    private DeviceField deviceField1;
    private DeviceField deviceField2;
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private DeviceField deviceField7;
    private DeviceField deviceField8;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    // End of variables declaration//GEN-END:variables
}