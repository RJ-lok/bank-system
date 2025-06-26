import java.sql.*;
import java.util.Scanner;

public class BankApp {
    static final String DB_URL = "jdbc:postgresql://localhost:5432/bank";
    static final String USER = "postgres";
    static final String PASS = "your_new_password";

    static Scanner scanner = new Scanner(System.in);

    public static void createAccount() {
        System.out.print("Enter Your Name: ");
        String name = scanner.next();
        System.out.print("Which Type Of Account (Current/Saving): ");
        String type = scanner.next();
        System.out.print("Enter Mobile Number: ");
        long mobile = scanner.nextLong();
        scanner.nextLine();
        System.out.print("Enter Address: ");
        String address = scanner.nextLine();
        System.out.print("Enter Amount to Deposit: ");
        float balance = scanner.nextFloat();

        try (Connection conn = DriverManager.getConnection(DB_URL, USER, PASS);
             Statement stmt = conn.createStatement()) {

            int lastCustomerId = 3154;
            long lastAccountNumber = 867578622952L;

            String fetchQuery = "SELECT customer_id, account_number FROM bank ORDER BY customer_id DESC LIMIT 1";
            ResultSet rs = stmt.executeQuery(fetchQuery);
            if (rs.next()) {
                lastCustomerId = rs.getInt("customer_id");
                lastAccountNumber = rs.getLong("account_number");
            }

            int newCustomerId = lastCustomerId + 1;
            long newAccountNumber = lastAccountNumber + 1;

            String insertQuery = "INSERT INTO bank (customer_id, account_number, customer_name, account_type, address, mobile, balance) VALUES (?, ?, ?, ?, ?, ?, ?)";
            PreparedStatement ps = conn.prepareStatement(insertQuery);
            ps.setInt(1, newCustomerId);
            ps.setLong(2, newAccountNumber);
            ps.setString(3, name);
            ps.setString(4, type);
            ps.setString(5, address);
            ps.setLong(6, mobile);
            ps.setFloat(7, balance);

            ps.executeUpdate();
            System.out.println("Account created successfully. Customer ID: " + newCustomerId);

        } catch (SQLException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }

    public static void checkBalanceById() {
        System.out.print("Enter Customer ID to check balance: ");
        int id = scanner.nextInt();

        try (Connection conn = DriverManager.getConnection(DB_URL, USER, PASS);
             Statement stmt = conn.createStatement()) {

            String query = "SELECT customer_name, balance FROM bank WHERE customer_id = " + id;
            ResultSet rs = stmt.executeQuery(query);
            if (rs.next()) {
                System.out.println("Hello " + rs.getString("customer_name") +
                        ", your balance is: ₹" + rs.getFloat("balance"));
            } else {
                System.out.println("Customer not found.");
            }

        } catch (SQLException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }

    public static void changeMobileById() {
        System.out.print("Enter Customer ID to change mobile number: ");
        int id = scanner.nextInt();
        System.out.print("Enter New Mobile Number: ");
        long newMobile = scanner.nextLong();

        try (Connection conn = DriverManager.getConnection(DB_URL, USER, PASS)) {
            String update = "UPDATE bank SET mobile = ? WHERE customer_id = ?";
            PreparedStatement ps = conn.prepareStatement(update);
            ps.setLong(1, newMobile);
            ps.setInt(2, id);

            int rows = ps.executeUpdate();
            if (rows > 0) {
                System.out.println("Mobile number updated successfully.");
            } else {
                System.out.println("Update failed. Customer ID not found.");
            }

        } catch (SQLException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }

    public static void transferMoney() {
        System.out.print("Enter Sender Customer ID: ");
        int senderId = scanner.nextInt();
        System.out.print("Enter Receiver Customer ID: ");
        int receiverId = scanner.nextInt();
        System.out.print("Enter Amount to Transfer: ");
        float amount = scanner.nextFloat();

        try (Connection conn = DriverManager.getConnection(DB_URL, USER, PASS)) {
            conn.setAutoCommit(false);

            String balanceQuery = "SELECT balance FROM bank WHERE customer_id = ?";
            PreparedStatement psSender = conn.prepareStatement(balanceQuery);
            psSender.setInt(1, senderId);
            ResultSet rsSender = psSender.executeQuery();

            if (!rsSender.next()) {
                System.out.println("Sender not found.");
                return;
            }

            float senderBalance = rsSender.getFloat("balance");
            if (senderBalance < amount) {
                System.out.println("Insufficient balance.");
                return;
            }

            String debit = "UPDATE bank SET balance = balance - ? WHERE customer_id = ?";
            String credit = "UPDATE bank SET balance = balance + ? WHERE customer_id = ?";

            PreparedStatement psDebit = conn.prepareStatement(debit);
            psDebit.setFloat(1, amount);
            psDebit.setInt(2, senderId);

            PreparedStatement psCredit = conn.prepareStatement(credit);
            psCredit.setFloat(1, amount);
            psCredit.setInt(2, receiverId);

            int d = psDebit.executeUpdate();
            int c = psCredit.executeUpdate();

            if (d > 0 && c > 0) {
                conn.commit();
                System.out.println("Transfer successful.");
            } else {
                conn.rollback();
                System.out.println("Transfer failed. Check customer IDs.");
            }

        } catch (SQLException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        int choice = 0;
        while (choice != 5) {
            System.out.println("\n====== Bank Menu ======");
            System.out.println("1. Create Account");
            System.out.println("2. Check Balance by Customer ID");
            System.out.println("3. Change Mobile Number by Customer ID");
            System.out.println("4. Transfer Money between Customers");
            System.out.println("5. Exit");
            System.out.print("Enter your choice: ");
            choice = scanner.nextInt();

            switch (choice) {
                case 1 -> createAccount();
                case 2 -> checkBalanceById();
                case 3 -> changeMobileById();
                case 4 -> transferMoney();
                case 5 -> System.out.println("Goodbye!");
                default -> System.out.println("Invalid option. Try again.");
            }
        }
    }
}
